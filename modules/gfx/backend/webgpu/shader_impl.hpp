#pragma once
#include <toolbox/gfx/api/shader.hpp>

#include <webgpu/webgpu_cpp.h>

namespace ct::gfx {
class Device;
}

namespace ct::gfx::webgpu {

class ShaderModuleImpl final : public ShaderModule {
public:
    ShaderModuleImpl() = default;
    ~ShaderModuleImpl() override = default;

    bool InitFromWGSL(ref<Device> device, const std::string& wgsl, const ShaderDesc& desc) noexcept;
    bool InitFromFile(ref<Device> device, const std::string& path, const ShaderFromFileDesc& desc) noexcept;

    [[nodiscard]] ShaderStage GetStage() const noexcept override { return mStage; }
    [[nodiscard]] const std::string& GetEntryPoint() const noexcept override { return mEntryPoint; }

    [[nodiscard]] void* GetNativeModule() const noexcept override { return (void*)mModule.Get(); }
    [[nodiscard]] const wgpu::ShaderModule& Handle() const noexcept { return mModule; }

private:
    static bool ReadFileToString(const std::string& path, std::string& out) noexcept;

private:
    wgpu::Device mDevice;
    wgpu::ShaderModule mModule;

    ShaderStage mStage{ShaderStage::Vertex};
    std::string mEntryPoint{"main"};
};

} // namespace ct::gfx::webgpu
