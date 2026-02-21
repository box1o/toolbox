#pragma once
#include <toolbox/base/base.hpp>

namespace ct::gfx {

class Device;

enum class ShaderStage : u8 { Vertex, Fragment, Compute };

struct ShaderDesc {
    ShaderStage stage{ShaderStage::Vertex};
    std::string entryPoint{"main"};
    std::string debugName{"Shader"};
};

struct ShaderFromFileDesc {
    ShaderStage stage{ShaderStage::Vertex};
    std::string entryPoint{"main"};
    std::string debugName{};
};

class ShaderModule {
public:
    virtual ~ShaderModule() = default;

    [[nodiscard]] virtual ShaderStage GetStage() const noexcept = 0;
    [[nodiscard]] virtual const std::string& GetEntryPoint() const noexcept = 0;
    [[nodiscard]] virtual void* GetNativeModule() const noexcept = 0;

    [[nodiscard]] static result<ref<ShaderModule>> FromWGSL(
        ref<Device> device, const std::string& wgsl, const ShaderDesc& desc) noexcept;

    [[nodiscard]] static result<ref<ShaderModule>> FromFile(
        ref<Device> device, const std::string& path, const ShaderFromFileDesc& desc) noexcept;

protected:
    ShaderModule() = default;
};

} // namespace ct::gfx
