#pragma once
#include <toolbox/gfx/api/shader.hpp>
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx::webgpu {

class ShaderImpl final : public Shader {
public:
    explicit ShaderImpl(ref<Device> device, const ShaderDesc& desc);
    ~ShaderImpl() override = default;

    // clang-format off
    void SetStages(std::vector<ShaderStageInfo> stages)  { mStages = std::move(stages); }
    void SetIncludePaths(std::vector<Path> includePaths) { mIncludePaths = std::move(includePaths); }
    void EnableReflection(bool enableReflection)         { mEnableReflection = enableReflection; }
    void EnableCache(bool enableCache)                   { mEnableCache = enableCache; }

    [[nodiscard]] const std::string& GetEntryPoint() const noexcept override { return mDesc.entryPoint; }
    [[nodiscard]] void* GetNativeShaderHandle() const noexcept override { return static_cast<void*>(&mShaderModule); }

    // clang-format on

    result<void> Initialize() noexcept override;

private:
    result<std::string> LoadStageSource(const ShaderStageInfo& stage) const noexcept;
    result<std::string> MergeStages() const noexcept;
    result<void> CompileShaderModule(const std::string& wgslSource) noexcept;

private:
    ShaderDesc mDesc{};
    ref<Device> mDevice{nullptr};
    mutable wgpu::ShaderModule mShaderModule{nullptr};
    std::vector<ShaderStageInfo> mStages{};
    std::vector<Path> mIncludePaths{};
    bool mEnableReflection{false};
    bool mEnableCache{false};
};

} // namespace ct::gfx::webgpu
