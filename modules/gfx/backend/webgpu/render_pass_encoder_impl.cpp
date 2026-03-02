#include "render_pass_encoder_impl.hpp"
#include "toolbox/gfx/api/pipeline.hpp"
#include "toolbox/gfx/api/buffer.hpp"
#include "toolbox/gfx/api/bind_group.hpp"
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx::webgpu {

RenderPassEncoderImpl::RenderPassEncoderImpl(wgpu::RenderPassEncoder encoder)
    : mEncoder(std::move(encoder)) {}

void RenderPassEncoderImpl::SetPipeline(ref<Pipeline> pipeline) noexcept {
    if (!mEncoder || !pipeline) return;

    auto* nativePipeline = static_cast<wgpu::RenderPipeline*>(pipeline->GetNativePipelineHandle());
    if (!nativePipeline || !*nativePipeline) return;

    mEncoder.SetPipeline(*nativePipeline);
}

void RenderPassEncoderImpl::SetBindGroup(
    u32 groupIndex, ref<BindGroup> group,
    u32 dynamicOffsetCount, const u32* dynamicOffsets) noexcept {
    if (!mEncoder || !group) return;

    auto* nativeGroup = static_cast<wgpu::BindGroup*>(group->GetNativeHandle());
    if (!nativeGroup || !*nativeGroup) return;

    mEncoder.SetBindGroup(groupIndex, *nativeGroup, dynamicOffsetCount, dynamicOffsets);
}

void RenderPassEncoderImpl::SetVertexBuffer(
    u32 slot, ref<Buffer> buffer, u64 offset, u64 size) noexcept {
    if (!mEncoder || !buffer) return;

    auto* nativeBuffer = static_cast<wgpu::Buffer*>(buffer->GetNativeBufferHandle());
    if (!nativeBuffer || !*nativeBuffer) return;

    u64 effectiveSize = (size == 0) ? (buffer->GetSize() - offset) : size;
    mEncoder.SetVertexBuffer(slot, *nativeBuffer,
        static_cast<uint64_t>(offset), static_cast<uint64_t>(effectiveSize));
}

void RenderPassEncoderImpl::SetIndexBuffer(
    ref<Buffer> buffer, IndexFormat fmt, u64 offset, u64 size) noexcept {
    if (!mEncoder || !buffer) return;

    auto* nativeBuffer = static_cast<wgpu::Buffer*>(buffer->GetNativeBufferHandle());
    if (!nativeBuffer || !*nativeBuffer) return;

    wgpu::IndexFormat wgpuFmt = (fmt == IndexFormat::Uint16)
        ? wgpu::IndexFormat::Uint16
        : wgpu::IndexFormat::Uint32;

    u64 effectiveSize = (size == 0) ? (buffer->GetSize() - offset) : size;
    mEncoder.SetIndexBuffer(*nativeBuffer, wgpuFmt,
        static_cast<uint64_t>(offset), static_cast<uint64_t>(effectiveSize));
}

void RenderPassEncoderImpl::SetViewport(const ViewportDesc& viewport) noexcept {
    if (!mEncoder) return;
    mEncoder.SetViewport(viewport.x, viewport.y, viewport.width, viewport.height,
        viewport.minDepth, viewport.maxDepth);
}

void RenderPassEncoderImpl::SetScissorRect(const ScissorDesc& scissor) noexcept {
    if (!mEncoder) return;
    mEncoder.SetScissorRect(scissor.x, scissor.y, scissor.width, scissor.height);
}

void RenderPassEncoderImpl::SetBlendConstant(f32 r, f32 g, f32 b, f32 a) noexcept {
    if (!mEncoder) return;
    wgpu::Color color{};
    color.r = static_cast<double>(r);
    color.g = static_cast<double>(g);
    color.b = static_cast<double>(b);
    color.a = static_cast<double>(a);
    mEncoder.SetBlendConstant(&color);
}

void RenderPassEncoderImpl::SetStencilReference(u32 reference) noexcept {
    if (!mEncoder) return;
    mEncoder.SetStencilReference(reference);
}

void RenderPassEncoderImpl::Draw(const DrawDesc& desc) noexcept {
    if (!mEncoder) return;
    mEncoder.Draw(desc.vertexCount, desc.instanceCount, desc.firstVertex, desc.firstInstance);
}

void RenderPassEncoderImpl::DrawIndexed(const DrawIndexedDesc& desc) noexcept {
    if (!mEncoder) return;
    mEncoder.DrawIndexed(desc.indexCount, desc.instanceCount, desc.firstIndex,
        desc.vertexOffset, desc.firstInstance);
}

void RenderPassEncoderImpl::End() noexcept {
    if (!mEncoder || mEnded) return;
    mEncoder.End();
    mEnded = true;
}

} // namespace ct::gfx::webgpu
