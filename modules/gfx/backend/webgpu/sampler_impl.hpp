#pragma once
#include <toolbox/gfx/api/sampler.hpp>

#include <webgpu/webgpu_cpp.h>

namespace ct::gfx {
class Device;
}

namespace ct::gfx::webgpu {

class SamplerImpl final : public Sampler {
public:
    SamplerImpl() = default;
    ~SamplerImpl() override = default;

    bool Init(ref<Device> device, const SamplerDesc& desc) noexcept;

    [[nodiscard]] void* GetNativeSampler() const noexcept override { return (void*)mSampler.Get(); }
    [[nodiscard]] const wgpu::Sampler& Handle() const noexcept { return mSampler; }

private:
    static wgpu::FilterMode ToWGPU(FilterMode f) noexcept;
    static wgpu::AddressMode ToWGPU(AddressMode a) noexcept;

private:
    wgpu::Device mDevice;
    wgpu::Sampler mSampler;
};

} // namespace ct::gfx::webgpu
