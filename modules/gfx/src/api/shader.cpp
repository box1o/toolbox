#include "toolbox/gfx/api/shader.hpp"


#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/shader_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Shader>> Shader::Builder::Build() noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto shader = createRef<webgpu::ShaderImpl>(device, desc);
    shader->SetStages(stages);
    shader->SetIncludePaths(includePaths);
    shader->EnableReflection(enableReflection);
    shader->EnableCache(enableCache);
    TRY_RETURN(shader->Initialize());
    return ok(std::move(shader));
#else
    log::Critical("Shader creation failed: no graphics backend available");
    std::abort();
#endif
}

// NOTE: Builder
Shader::Builder& Shader::Builder::AddShaderStage(ShaderStage stage, const Path& path) {
    ShaderStageInfo info{};
    info.stage  = stage;
    info.path   = path;
    info.name   = "shader_stage_" + std::to_string(static_cast<u8>(stage));
    info.source = {};
    info.isFile = true;
    stages.push_back(std::move(info));
    return *this;
}

Shader::Builder& Shader::Builder::AddStageSource(
    ShaderStage stage, std::string source, std::string name) {
    ShaderStageInfo info{};
    info.stage  = stage;
    info.path   = Path{};
    info.name   = std::move(name);
    info.source = std::move(source);
    info.isFile = false;
    stages.push_back(std::move(info));
    return *this;
}

Shader::Builder& Shader::Builder::AddShaderFile(ShaderStage stages_, const Path& path) {
    // NOTE: Combined stages (e.g. Vertex | Fragment) stored in a single entry
    ShaderStageInfo info{};
    info.stage  = stages_;
    info.path   = path;
    info.name   = path.stem().string();
    info.source = {};
    info.isFile = true;
    stages.push_back(std::move(info));
    return *this;
}

Shader::Builder& Shader::Builder::AddShaderSource(
    ShaderStage stages_, std::string source, std::string name) {
    // NOTE: Combined stages from inline source
    ShaderStageInfo info{};
    info.stage  = stages_;
    info.path   = Path{};
    info.name   = std::move(name);
    info.source = std::move(source);
    info.isFile = false;
    stages.push_back(std::move(info));
    return *this;
}

Shader::Builder& Shader::Builder::AddIncludePath(const Path& path) {
    includePaths.push_back(path);
    return *this;
}

Shader::Builder& Shader::Builder::EnableReflection(bool enable) {
    enableReflection = enable;
    return *this;
}

Shader::Builder& Shader::Builder::EnableCache(bool enable) {
    enableCache = enable;
    return *this;
}

} // namespace ct::gfx
