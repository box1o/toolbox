#include "sampler_impl.hpp"
#include "device_impl.hpp"

#include <toolbox/base/logger/logger.hpp>

namespace ct::gfx::webgpu {

wgpu::FilterMode SamplerImpl::ToWGPU(FilterMode f) noexcept {
    return (f == FilterMode::Nearest) ? wgpu::FilterMode::Nearest : wgpu::FilterMode::Linear;
}

wgpu::AddressMode SamplerImpl::ToWGPU(AddressMode a) noexcept {
    switch (a) {
        case AddressMode::Repeat:       return wgpu::AddressMode::Repeat;
        case AddressMode::MirrorRepeat: return wgpu::AddressMode::MirrorRepeat;
        case AddressMode::ClampToEdge:  return wgpu::AddressMode::ClampToEdge;
        default:                        return wgpu::AddressMode::ClampToEdge;
    }
}

bool SamplerImpl::Init(ref<Device> device, const SamplerDesc& desc) noexcept {
    if (!device) return false;

    auto* dev = dynamic_cast<DeviceImpl*>(device.get());
    if (!dev) {
        log::Error("SamplerImpl: device is not WebGPU device");
        return false;
    }

    mDevice = dev->DeviceHandle();
    if (!mDevice) return false;

    wgpu::SamplerDescriptor sd{};
    sd.minFilter = ToWGPU(desc.minFilter);
    sd.magFilter = ToWGPU(desc.magFilter);
    sd.mipmapFilter = wgpu::MipmapFilterMode::Linear;

    sd.addressModeU = ToWGPU(desc.addressU);
    sd.addressModeV = ToWGPU(desc.addressV);
    sd.addressModeW = ToWGPU(desc.addressW);

    if (!desc.debugName.empty()) sd.label = desc.debugName.c_str();

    mSampler = mDevice.CreateSampler(&sd);
    return mSampler != nullptr;
}

} // namespace ct::gfx::webgpu
