#pragma once
#include <toolbox/gfx/api/render_pass.hpp>
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx::webgpu {

class RenderPassEncoderImpl final : public RenderPassEncoder {
public:
    explicit RenderPassEncoderImpl(wgpu::RenderPassEncoder encoder);
    ~RenderPassEncoderImpl() override = default;

    void SetPipeline(ref<Pipeline> pipeline) noexcept override;
    void SetBindGroup(u32 groupIndex, ref<BindGroup> group, u32 dynamicOffsetCount, const u32* dynamicOffsets) noexcept override;
    void SetVertexBuffer(u32 slot, ref<Buffer> buffer, u64 offset, u64 size) noexcept override;
    void SetIndexBuffer(ref<Buffer> buffer, IndexFormat fmt, u64 offset, u64 size) noexcept override;

    void SetViewport(const ViewportDesc& viewport) noexcept override;
    void SetScissorRect(const ScissorDesc& scissor) noexcept override;
    void SetBlendConstant(f32 r, f32 g, f32 b, f32 a) noexcept override;
    void SetStencilReference(u32 reference) noexcept override;

    void Draw(const DrawDesc& desc) noexcept override;
    void DrawIndexed(const DrawIndexedDesc& desc) noexcept override;

    void End() noexcept override;

private:
    wgpu::RenderPassEncoder mEncoder{nullptr};
    bool mEnded{false};
};

} // namespace ct::gfx::webgpu
