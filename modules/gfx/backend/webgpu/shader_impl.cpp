#include "shader_impl.hpp"
#include "device_impl.hpp"

#include <toolbox/base/logger/logger.hpp>

#include <fstream>
#include <sstream>

namespace ct::gfx::webgpu {

bool ShaderModuleImpl::ReadFileToString(const std::string& path, std::string& out) noexcept {
    std::ifstream f(path, std::ios::in | std::ios::binary);
    if (!f.is_open()) return false;
    std::ostringstream ss;
    ss << f.rdbuf();
    out = ss.str();
    return !out.empty();
}

bool ShaderModuleImpl::InitFromWGSL(ref<Device> device, const std::string& wgsl, const ShaderDesc& desc) noexcept {
    if (!device) return false;
    if (wgsl.empty()) {
        log::Error("ShaderModuleImpl: WGSL source empty");
        return false;
    }

    auto* dev = dynamic_cast<DeviceImpl*>(device.get());
    if (!dev) {
        log::Error("ShaderModuleImpl: device is not WebGPU device");
        return false;
    }

    mDevice = dev->DeviceHandle();
    if (!mDevice) return false;

    mStage = desc.stage;
    mEntryPoint = desc.entryPoint.empty() ? "main" : desc.entryPoint;

    // Dawn C++ wrapper: use ShaderSourceWGSL as chained struct
    wgpu::ShaderSourceWGSL wgslSrc{};
    wgslSrc.code = wgsl.c_str();

    wgpu::ShaderModuleDescriptor smd{};
    smd.nextInChain = &wgslSrc;
    if (!desc.debugName.empty()) smd.label = desc.debugName.c_str();

    mModule = mDevice.CreateShaderModule(&smd);
    if (!mModule) {
        log::Error("ShaderModuleImpl: CreateShaderModule failed");
        return false;
    }

    return true;
}

bool ShaderModuleImpl::InitFromFile(ref<Device> device, const std::string& path, const ShaderFromFileDesc& desc) noexcept {
    std::string src;
    if (!ReadFileToString(path, src)) {
        log::Error("ShaderModuleImpl: failed to read file: {}", path);
        return false;
    }

    ShaderDesc d{};
    d.stage = desc.stage;
    d.entryPoint = desc.entryPoint;
    d.debugName = desc.debugName.empty() ? path : desc.debugName;

    return InitFromWGSL(device, src, d);
}

} // namespace ct::gfx::webgpu
