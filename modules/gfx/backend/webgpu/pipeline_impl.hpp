#pragma once
#include <toolbox/gfx/api/pipeline.hpp>

#include <webgpu/webgpu_cpp.h>

namespace ct::gfx {
class Device;
}

namespace ct::gfx::webgpu {

class RenderPipelineImpl final : public RenderPipeline {
public:
    RenderPipelineImpl() = default;
    ~RenderPipelineImpl() override = default;

    bool Init(ref<Device> device, const RenderPipelineDesc& desc) noexcept;

    [[nodiscard]] void* GetNativePipeline() const noexcept override { return (void*)mPipeline.Get(); }
    [[nodiscard]] const wgpu::RenderPipeline& Handle() const noexcept { return mPipeline; }

private:
    static wgpu::VertexFormat ToWGPU(VertexFormat f) noexcept;
    static wgpu::VertexStepMode ToWGPU(VertexInputRate r) noexcept;

    static wgpu::TextureFormat ToWGPU(TextureFormat f) noexcept;

    static wgpu::PrimitiveTopology ToWGPU(PrimitiveTopology t) noexcept;
    static wgpu::CullMode ToWGPU(CullMode c) noexcept;
    static wgpu::FrontFace ToWGPU(FrontFace f) noexcept;

private:
    wgpu::Device mDevice;
    wgpu::RenderPipeline mPipeline;
};

} // namespace ct::gfx::webgpu
