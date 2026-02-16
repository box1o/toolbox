#include "toolbox/gfx/api/sampler.hpp"
#include "toolbox/gfx/api/device.hpp"

namespace ct {

result<ref<Sampler>> Sampler::Create(ref<Device> device, const SamplerInfo& info) noexcept {
    if (!device) return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Device is null");

    ref<Sampler> sampler(new Sampler());

    if (!sampler->Init(*device, info)) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to create sampler");
    }

    log::Info("[wgpu] Sampler created");
    return sampler;
}

Sampler::~Sampler() {
    mSampler = nullptr;
}

bool Sampler::Init(const Device& device, const SamplerInfo& info) {
    mInfo = info;

    wgpu::SamplerDescriptor desc{};
    desc.minFilter = ToWGPU(info.minFilter);
    desc.magFilter = ToWGPU(info.magFilter);
    desc.mipmapFilter = ToWGPUMipmap(info.mipmapFilter);
    desc.addressModeU = ToWGPU(info.addressU);
    desc.addressModeV = ToWGPU(info.addressV);
    desc.addressModeW = ToWGPU(info.addressW);
    desc.maxAnisotropy = static_cast<u16>(info.maxAnisotropy);

    if (info.compare != CompareFunction::Undefined) {
        desc.compare = ToWGPU(info.compare);
    }

    mSampler = device.GetDevice().CreateSampler(&desc);
    return mSampler != nullptr;
}

const SamplerInfo& Sampler::GetInfo() const noexcept { return mInfo; }
wgpu::Sampler Sampler::GetHandle() const noexcept { return mSampler; }

} // namespace ct
