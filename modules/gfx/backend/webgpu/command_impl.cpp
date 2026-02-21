#include "command_impl.hpp"
#include "device_impl.hpp"

#include "pipeline_impl.hpp"
#include "buffer_impl.hpp"
#include "bindings_impl.hpp"

#include <toolbox/base/logger/logger.hpp>

namespace ct::gfx::webgpu {

static inline wgpu::IndexFormat ToWGPU(IndexFormat f) noexcept {
    return (f == IndexFormat::U32) ? wgpu::IndexFormat::Uint32 : wgpu::IndexFormat::Uint16;
}

static inline const wgpu::TextureView* AsViewPtr(void* p) {
    return static_cast<const wgpu::TextureView*>(p);
}

void RenderPassEncoderImpl::BindPipeline(ref<RenderPipeline> pipeline) noexcept {
    if (!mPass || !pipeline) return;
    auto* impl = dynamic_cast<RenderPipelineImpl*>(pipeline.get());
    if (!impl) return;
    mPass.SetPipeline(impl->Handle());
}

void RenderPassEncoderImpl::BindBindGroup(u32 setIndex, ref<BindGroup> group) noexcept {
    if (!mPass || !group) return;
    auto* impl = dynamic_cast<BindGroupImpl*>(group.get());
    if (!impl) return;
    mPass.SetBindGroup(setIndex, impl->Handle());
}

void RenderPassEncoderImpl::BindVertexBuffer(u32 slot, ref<Buffer> buffer, u64 offset) noexcept {
    if (!mPass || !buffer) return;
    auto* impl = dynamic_cast<BufferImpl*>(buffer.get());
    if (!impl) return;
    mPass.SetVertexBuffer(slot, impl->Handle(), offset);
}

void RenderPassEncoderImpl::BindIndexBuffer(ref<Buffer> buffer, IndexFormat fmt, u64 offset) noexcept {
    if (!mPass || !buffer) return;
    auto* impl = dynamic_cast<BufferImpl*>(buffer.get());
    if (!impl) return;
    mPass.SetIndexBuffer(impl->Handle(), ToWGPU(fmt), offset);
}

void RenderPassEncoderImpl::Draw(const DrawDesc& d) noexcept {
    if (!mPass || d.vertexCount == 0) return;
    mPass.Draw(d.vertexCount, d.instanceCount, d.firstVertex, d.firstInstance);
}

void RenderPassEncoderImpl::DrawIndexed(const DrawIndexedDesc& d) noexcept {
    if (!mPass || d.indexCount == 0) return;
    mPass.DrawIndexed(d.indexCount, d.instanceCount, d.firstIndex, d.vertexOffset, d.firstInstance);
}

bool CommandEncoderImpl::Init(ref<Device> device, const CommandEncoderDesc& desc) noexcept {
    if (!device) return false;

    auto* dev = dynamic_cast<DeviceImpl*>(device.get());
    if (!dev) {
        log::Error("CommandEncoderImpl: device is not WebGPU device");
        return false;
    }

    mDevice = dev->DeviceHandle();
    if (!mDevice) return false;

    wgpu::CommandEncoderDescriptor ced{};
    if (!desc.debugName.empty()) ced.label = desc.debugName.c_str();

    mEncoder = mDevice.CreateCommandEncoder(&ced);
    mFinished = false;

    mKeepAliveViews.clear();
    mColorAttachments.clear();

    return mEncoder != nullptr;
}

result<ref<RenderPassEncoder>> CommandEncoderImpl::BeginRenderPass(const RenderPassDesc& desc) noexcept {
    if (!mEncoder || mFinished) {
        return err(ErrorCode::INVALID_STATE, "CommandEncoder: invalid state");
    }
    if (desc.colors.empty()) {
        return err(ErrorCode::INVALID_ARGUMENT, "RenderPass: no color attachments");
    }

    mColorAttachments.clear();
    mColorAttachments.reserve(desc.colors.size());

    for (const auto& c : desc.colors) {
        if (!c.view) {
            return err(ErrorCode::INVALID_ARGUMENT, "RenderPass: color attachment view is null");
        }

        const wgpu::TextureView* ptr = AsViewPtr(c.view);
        if (!ptr || !(*ptr)) {
            return err(ErrorCode::INVALID_ARGUMENT, "RenderPass: invalid color view pointer");
        }

        wgpu::TextureView view = *ptr;
        mKeepAliveViews.push_back(view);

        wgpu::RenderPassColorAttachment ca{};
        ca.view = view;
        ca.loadOp = c.clearEnabled ? wgpu::LoadOp::Clear : wgpu::LoadOp::Load;
        ca.storeOp = wgpu::StoreOp::Store;
        ca.clearValue = { c.clear.r, c.clear.g, c.clear.b, c.clear.a };

        mColorAttachments.push_back(ca);
    }

    wgpu::RenderPassDescriptor rp{};
    rp.colorAttachmentCount = (uint32_t)mColorAttachments.size();
    rp.colorAttachments = mColorAttachments.data();

    wgpu::RenderPassDepthStencilAttachment depth{};
    wgpu::TextureView depthView{nullptr};

    if (desc.enableDepth) {
        if (!desc.depthView) {
            return err(ErrorCode::INVALID_ARGUMENT, "RenderPass: depth enabled but depthView null");
        }

        const wgpu::TextureView* dptr = AsViewPtr(desc.depthView);
        if (!dptr || !(*dptr)) {
            return err(ErrorCode::INVALID_ARGUMENT, "RenderPass: invalid depth view pointer");
        }

        depthView = *dptr;
        mKeepAliveViews.push_back(depthView);

        depth.view = depthView;

        depth.depthLoadOp = wgpu::LoadOp::Clear;
        depth.depthStoreOp = wgpu::StoreOp::Store;
        depth.depthClearValue = desc.clearDepth;

        depth.stencilLoadOp = wgpu::LoadOp::Clear;
        depth.stencilStoreOp = wgpu::StoreOp::Store;
        depth.stencilClearValue = desc.clearStencil;

        rp.depthStencilAttachment = &depth;
    }

    auto pass = mEncoder.BeginRenderPass(&rp);
    if (!pass) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "BeginRenderPass failed");
    }

    return ok(createRef<RenderPassEncoderImpl>(pass));
}

result<ref<CommandBuffer>> CommandEncoderImpl::Finish() noexcept {
    if (!mEncoder || mFinished) {
        return err(ErrorCode::INVALID_STATE, "CommandEncoder: already finished");
    }

    mFinished = true;

    wgpu::CommandBufferDescriptor cbd{};
    auto buf = mEncoder.Finish(&cbd);
    if (!buf) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Finish command buffer failed");
    }

    mEncoder = nullptr;
    mKeepAliveViews.clear();
    mColorAttachments.clear();

    return ok(createRef<CommandBufferImpl>(buf));
}

} // namespace ct::gfx::webgpu
