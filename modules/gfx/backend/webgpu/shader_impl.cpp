#include "shader_impl.hpp"
#include "common.hpp"
#include "toolbox/gfx/api/device.hpp"

#include <fstream>
#include <sstream>
#include <unordered_set>

#if defined(WEBGPU_BACKEND_EMSCRIPTEN)
#include <emscripten/emscripten.h>
#endif

namespace ct::gfx::webgpu {

namespace {

struct ShaderCompileState {
    std::atomic<bool> done{false};
    bool hasErrors{false};
    std::string messages{};
};

static void PumpUntilDone(wgpu::Instance* instance, const std::atomic<bool>& doneFlag) {
#if defined(WEBGPU_BACKEND_EMSCRIPTEN)
    while (!doneFlag.load(std::memory_order_acquire)) emscripten_sleep(1);
#else
    while (!doneFlag.load(std::memory_order_acquire)) {
        if (instance && *instance) instance->ProcessEvents();
    }
#endif
}

static constexpr std::string_view kIncludeExtension = ".include";

} // namespace

// NOTE: ShaderImpl
ShaderImpl::ShaderImpl(ref<Device> device, const ShaderDesc& desc)
    : mDesc(desc), mDevice(std::move(device)) {}

result<void> ShaderImpl::Initialize() noexcept {
    if (mStages.empty())
        return err(
            ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Shader: no shader stages provided");

    if (mEnableReflection) log::Warn("[wgpu] Shader reflection is not supported; ignoring");
    if (mEnableCache) log::Warn("[wgpu] Shader cache is not supported; ignoring");

    for (const auto& path : mIncludePaths) {
        if (path.extension() != kIncludeExtension) {
            auto error = std::string("Shader: include path '") + path.string()
                       + "' must have a '" + std::string(kIncludeExtension)
                       + "' extension";
            return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, std::move(error));
        }
    }

    if (!mIncludePaths.empty())
        log::Warn("[wgpu] Shader include paths are not supported; ignoring {} path(s)",
            mIncludePaths.size());

    auto mergeRes = MergeStages();
    if (!mergeRes) return err(mergeRes.error());

    return CompileShaderModule(mergeRes.value());
}

result<std::string> ShaderImpl::LoadStageSource(const ShaderStageInfo& stage) const noexcept {
    if (!stage.isFile) {
        if (stage.source.empty())
            log::Warn("[wgpu] Shader stage '{}' ({}) has empty inline source", stage.name, detail::StageFlagsToString(stage.stage));
        return ok(stage.source);
    }

    if (stage.path.empty()) {
        auto error = std::string("Shader: stage '") + stage.name
                   + "' is marked as file but has empty path";
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, std::move(error));
    }

    std::ifstream file(stage.path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        auto error = std::string("Shader: failed to open file '") + stage.path.string()
                   + "' for stage '" + stage.name + "'";
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, std::move(error));
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    if (file.fail() && !file.eof()) {
        auto error = std::string("Shader: I/O error reading '") + stage.path.string()
                   + "' for stage '" + stage.name + "'";
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, std::move(error));
    }

    return ok(ss.str());
}

result<std::string> ShaderImpl::MergeStages() const noexcept {
    std::string merged;
    std::unordered_set<std::string> seenPaths{};
    std::unordered_set<std::size_t> seenHashes{};

    merged.reserve(4096);

    for (const auto& stage : mStages) {
        if (stage.isFile && !stage.path.empty()) {
            const std::string canonical = stage.path.lexically_normal().string();
            if (seenPaths.count(canonical)) continue;
            seenPaths.insert(canonical);
        }

        auto srcRes = LoadStageSource(stage);
        if (!srcRes) return err(srcRes.error());

        const std::string& src = srcRes.value();

        if (!stage.isFile) {
            const std::size_t h = std::hash<std::string>{}(src);
            if (seenHashes.count(h)) continue;
            seenHashes.insert(h);
        }

        if (!merged.empty()) merged += "\n\n";
        merged += src;
    }

    if (merged.empty())
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "Shader: all stages were deduplicated — nothing left to compile");

    return ok(std::move(merged));
}

result<void> ShaderImpl::CompileShaderModule(const std::string& wgslSource) noexcept {
    if (!mDevice)
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Shader: device handle is null");

    auto* nativeDevice = static_cast<wgpu::Device*>(mDevice->GetNativeDeviceHandle());
    if (!nativeDevice || !*nativeDevice)
        return err(
            ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Shader: native wgpu::Device is null");

    wgpu::ShaderSourceWGSL wgslDesc{};
    wgslDesc.code = wgpu::StringView{wgslSource.c_str(), wgslSource.size()};

    wgpu::ShaderModuleDescriptor smd{};
    smd.label = wgpu::StringView{mStages.empty() ? "shader" : mStages[0].name.c_str()};
    smd.nextInChain = &wgslDesc;

    mShaderModule = nativeDevice->CreateShaderModule(&smd);
    if (!mShaderModule)
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "Shader: wgpu::Device::CreateShaderModule returned null");

    auto state = std::make_shared<ShaderCompileState>();
    ShaderCompileState* rawState = state.get();

    mShaderModule.GetCompilationInfo(
        wgpu::CallbackMode::AllowSpontaneous,
        [](wgpu::CompilationInfoRequestStatus status, wgpu::CompilationInfo const* info,
            ShaderCompileState* s) {
            if (status != wgpu::CompilationInfoRequestStatus::Success) {
                s->hasErrors = true;
                s->messages = "CompilationInfo request failed";
                s->done.store(true, std::memory_order_release);
                return;
            }

            std::ostringstream ss;
            for (size_t i = 0; i < info->messageCount; ++i) {
                const auto& msg = info->messages[i];
                const std::string text = detail::ToString(msg.message);

                if (msg.type == wgpu::CompilationMessageType::Error) {
                    s->hasErrors = true;
                    ss << "[error] line " << msg.lineNum << ":" << msg.linePos << " — " << text
                       << '\n';
                } else if (msg.type == wgpu::CompilationMessageType::Warning) {
                    log::Warn(
                        "[wgpu] Shader warning — line {}:{} — {}", msg.lineNum, msg.linePos, text);
                }
            }

            if (s->hasErrors) s->messages = ss.str();
            s->done.store(true, std::memory_order_release);
        },
        rawState);

    auto* nativeInstance = static_cast<wgpu::Instance*>(mDevice->GetNativeInstanceHandle());
    PumpUntilDone(nativeInstance, state->done);

    if (state->hasErrors) {
        log::Error("[wgpu] Shader compilation failed:\n{}", state->messages);
        mShaderModule = nullptr;
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "Shader: WGSL compilation errors (see log above)");
    }

    return ok();
}

} // namespace ct::gfx::webgpu
