#pragma once
#include <toolbox/gfx/api/sampler.hpp>
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx::webgpu {

class SamplerImpl final : public Sampler {
public:
    explicit SamplerImpl(ref<Device> device, const SamplerDesc& desc);
    ~SamplerImpl() override = default;

    [[nodiscard]] void* GetNativeSamplerHandle() noexcept override { return static_cast<void*>(&mSampler); };

    // NOTE: Initialize
    result<void> Initialize() noexcept override;

private:
private:
    SamplerDesc mDesc{};

    ref<Device> mDevice{nullptr};
    wgpu::Sampler mSampler;
};

} // namespace ct::gfx::webgpu
