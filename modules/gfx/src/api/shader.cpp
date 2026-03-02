#include "toolbox/gfx/api/shader.hpp"

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/shader_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Shader>> Shader::Builder::Build() noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto shader = createRef<webgpu::ShaderImpl>(mDevice, mDesc);
    shader->SetStages(mStages);
    shader->SetIncludePaths(mIncludePaths);
    shader->EnableReflection(mEnableReflection);
    shader->EnableCache(mEnableCache);
    TRY_RETURN(shader->Initialize());
    return ok(std::move(shader));
#else
    log::Critical("Shader creation failed: no graphics backend available");
    std::abort();
#endif
}

Shader::Builder& Shader::Builder::AddShaderStage(ShaderStage stage, const Path& path) {
    ShaderStageInfo info{};
    info.stage = stage;
    info.path = path;
    info.name = "shader_stage_" + std::to_string(static_cast<u8>(stage));
    info.source = {};
    info.isFile = true;
    mStages.push_back(std::move(info));
    return *this;
}

Shader::Builder& Shader::Builder::AddStageSource(
    ShaderStage stage, std::string source, std::string name) {
    ShaderStageInfo info{};
    info.stage = stage;
    info.path = Path{};
    info.name = std::move(name);
    info.source = std::move(source);
    info.isFile = false;
    mStages.push_back(std::move(info));
    return *this;
}

Shader::Builder& Shader::Builder::AddShaderFile(ShaderStage stages, const Path& path) {
    ShaderStageInfo info{};
    info.stage = stages;
    info.path = path;
    info.name = path.stem().string();
    info.source = {};
    info.isFile = true;
    mStages.push_back(std::move(info));
    return *this;
}

Shader::Builder& Shader::Builder::AddShaderSource(
    ShaderStage stages, std::string source, std::string name) {
    ShaderStageInfo info{};
    info.stage = stages;
    info.path = Path{};
    info.name = std::move(name);
    info.source = std::move(source);
    info.isFile = false;
    mStages.push_back(std::move(info));
    return *this;
}

Shader::Builder& Shader::Builder::AddIncludePath(const Path& path) {
    mIncludePaths.push_back(path);
    return *this;
}

Shader::Builder& Shader::Builder::EnableReflection(bool enable) {
    mEnableReflection = enable;
    return *this;
}

Shader::Builder& Shader::Builder::EnableCache(bool enable) {
    mEnableCache = enable;
    return *this;
}

} // namespace ct::gfx
