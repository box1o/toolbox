#include "../../../include/toolbox/gfx/renderer/library/shaders.hpp"
#include "toolbox/base/errors/result.hpp"
#include "toolbox/base/logger/logger.hpp"
#include "toolbox/gfx/api/shader.hpp"

namespace ct::gfx {
ShaderLibrary::ShaderLibrary(ref<Device> device) : mDevice(device) {}
ShaderLibrary::~ShaderLibrary() {}

result<void> ShaderLibrary::Initialize() noexcept {
    TRY_RETURN(CreateBuiltinShaders());

    return ok();
}

static std::string kShaderDir{"resources/shaders/"};

result<void> ShaderLibrary::CreateBuiltinShaders() noexcept {

    auto path = kShaderDir + "unlit/wgsl";

    mBuiltins.at(static_cast<u32>(BuiltinShader::Unlit)) = Load(
        kShaderDir + "unlit.wgsl", ShaderStage::Vertex | ShaderStage::Vertex, ShaderFeature::None);

    return ok();
}

ShaderHandle ShaderLibrary::Load(const Path& path, ShaderStage stages, ShaderFeature features) {
    auto it = mNameCache.find(path.filename());
    if (it != mNameCache.end() && mShaders.IsValid(it->second)) {
        return it->second;
    }

    ShaderHandle handle = mShaders.Allocate();
    ShaderData* data = mShaders.Get(handle);

    auto shader = gfx::Shader::Create(mDevice).AddShaderFile(stages, path).Build();

    if (!shader.has_value()) {
        log::Critical("Failed to load shader {}", path.string());
    }

    data->shader = detail::unwrap(std::move(shader));

    if (!data->shader) {
        mShaders.Free(handle);
        return ShaderHandle{};
    }

    data->features = features;
    auto name = path.filename();
    data->name = name;

    // bool skinned =
    //     (static_cast<u32>(desc.features) & static_cast<u32>(ShaderFeature::Skinning)) != 0;
    // CreatePipelines(*data, skinned);

    mNameCache[name] = handle;
    return handle;
}

} // namespace ct::gfx
