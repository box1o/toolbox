#include "sampler_impl.hpp"
#include "toolbox/base/errors/errors.hpp"
#include "toolbox/gfx/api/device.hpp"
#include <webgpu/webgpu_cpp.h>

#include "common.hpp"

namespace ct::gfx::webgpu {

// NOTE: DeviceImpl
SamplerImpl::SamplerImpl(ref<Device> device, const SamplerDesc& desc)
    : mDesc(desc), mDevice(device) {
    if (!device) {
        log::Critical("Sampler creation failed: device is null");
        std::abort();
    }
}

result<void> SamplerImpl::Initialize() noexcept {
    if (!mDevice) {
        return err(ErrorCode::VALIDATION_NULL_VALUE, "SamplerImpl: device is null");
    };

    auto device = static_cast<wgpu::Device*>(mDevice->GetNativeDeviceHandle());
    if (!device) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "SamplerImpl: Filed to aquire the device handle");
    }

    wgpu::SamplerDescriptor sd{};
    sd.minFilter = detail::ToWGPU(mDesc.config.minFilter);
    sd.magFilter = detail::ToWGPU(mDesc.config.magFilter);
    sd.mipmapFilter = wgpu::MipmapFilterMode::Linear;

    sd.addressModeU = detail::ToWGPU(mDesc.config.wrapU);
    sd.addressModeV = detail::ToWGPU(mDesc.config.wrapV);
    sd.addressModeW = detail::ToWGPU(mDesc.config.wrapW);

    sd.maxAnisotropy = (u16)mDesc.config.maxAnisotropy;
    mSampler = device->CreateSampler(&sd);
    if (!mSampler) {
        return err(
            ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "SamplerImpl: failed to create sampler");
    }
    return ok();
}

} // namespace ct::gfx::webgpu
