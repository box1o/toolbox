#pragma once

#include <toolbox/base/base.hpp>

#include "../../api/shader.hpp"
#include "../handle.hpp"
#include "toolbox/gfx/renderer/resources/resource.hpp"

namespace ct::gfx {

enum class BuiltinShader : u32 {
    Unlit,
    // PBR,
    // PBRSkinned,
    // Shadow,
    // ShadowSkinned,
    // Depth,
    // Skybox,
    // Debug,
    Count
};

enum class ShaderFeature : u32 {
    None = 0,
    // Skinning = 1 << 0,
    // Instancing = 1 << 1,
    // NormalMapping = 1 << 2,
    // Shadows = 1 << 3,
    // IBL = 1 << 4,
    // Emission = 1 << 5
};

struct ShaderData {
    ref<gfx::Shader> shader;
    // scope<gfx::Pipeline> pipeline;
    // scope<gfx::Pipeline> pipelineInstanced;
    // scope<gfx::Pipeline> pipelineTwoSided;
    // scope<gfx::Pipeline> pipelineWireframe;
    ShaderFeature features{ShaderFeature::None};
    std::string name;
    bool valid = false;
};

inline ShaderFeature operator|(ShaderFeature a, ShaderFeature b) {
    return static_cast<ShaderFeature>(static_cast<u32>(a) | static_cast<u32>(b));
}

class Device;
class ShaderLibrary {
public:
    ShaderLibrary(ref<Device> device);
    ~ShaderLibrary();

    [[nodiscard]] ShaderHandle GetBuiltin(BuiltinShader bShader);
    [[nodiscard]] ShaderHandle FromFile(const Path& path, ShaderStage stage);

    [[nodiscard]] ref<Shader> GetShader(ShaderHandle handle);

    // FIXME:
    // void Reload();
    // void ReloadAll();

    ShaderHandle Load(const Path& path,ShaderStage stages,  ShaderFeature features = ShaderFeature::None);
    result<void> Initialize() noexcept;

private:
    result<void> CreateBuiltinShaders() noexcept;

private:
    ref<Device> mDevice{nullptr};
    std::unordered_map<std::string, ShaderHandle> mNameCache;
    ResourcePool<ShaderData, ShaderHandle> mShaders;
    std::array<ShaderHandle, static_cast<u32>(BuiltinShader::Count)> mBuiltins{};
};

} // namespace ct::gfx
