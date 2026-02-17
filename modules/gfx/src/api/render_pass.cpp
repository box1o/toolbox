#include "toolbox/gfx/api/render_pass.hpp"
#include "toolbox/gfx/api/device.hpp"
#include "toolbox/gfx/api/pipeline.hpp"
#include "toolbox/gfx/api/bind_group.hpp"
#include "toolbox/gfx/api/surface.hpp"

namespace ct {

namespace {

wgpu::IndexFormat ToWGPU(IndexFormat fmt) {
    switch (fmt) {
    case IndexFormat::U16: return wgpu::IndexFormat::Uint16;
    case IndexFormat::U32: return wgpu::IndexFormat::Uint32;
    }
    return wgpu::IndexFormat::Uint16;
}

} // namespace

RenderPass::RenderPass(RenderPass&& other) noexcept
    : mEncoder(std::move(other.mEncoder))
    , mPass(std::move(other.mPass))
    , mDevice(other.mDevice)
    , mSubmitted(other.mSubmitted) {
    other.mEncoder = nullptr;
    other.mPass = nullptr;
    other.mDevice = nullptr;
    other.mSubmitted = true;
}

RenderPass& RenderPass::operator=(RenderPass&& other) noexcept {
    if (this == &other) return *this;
    if (!mSubmitted && mPass) Submit();
    mEncoder = std::move(other.mEncoder);
    mPass = std::move(other.mPass);
    mDevice = other.mDevice;
    mSubmitted = other.mSubmitted;
    other.mEncoder = nullptr;
    other.mPass = nullptr;
    other.mDevice = nullptr;
    other.mSubmitted = true;
    return *this;
}

RenderPass::~RenderPass() {
    //NOTE: auto-submit if user forgot — prevents resource leak
    if (!mSubmitted && mPass) {
        Submit();
    }
}

result<RenderPass> RenderPass::Begin(Device& device, const RenderPassInfo& info) noexcept {
    if (!info.colorView) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Color view is null");
    }

    RenderPass pass;
    pass.mDevice = &device;
    pass.mSubmitted = false;

    wgpu::CommandEncoderDescriptor encDesc{};
    pass.mEncoder = device.GetDevice().CreateCommandEncoder(&encDesc);

    wgpu::RenderPassColorAttachment colorAttachment{};
    colorAttachment.view = info.colorView;
    colorAttachment.loadOp = info.loadColor ? wgpu::LoadOp::Load : wgpu::LoadOp::Clear;
    colorAttachment.storeOp = wgpu::StoreOp::Store;
    colorAttachment.clearValue = {info.clearColor.r, info.clearColor.g, info.clearColor.b, info.clearColor.a};

    wgpu::RenderPassDepthStencilAttachment depthAttachment{};
    bool hasDepth = info.depthView != nullptr;

    if (hasDepth) {
        depthAttachment.view = info.depthView;
        depthAttachment.depthLoadOp = info.loadDepth ? wgpu::LoadOp::Load : wgpu::LoadOp::Clear;
        depthAttachment.depthStoreOp = wgpu::StoreOp::Store;
        depthAttachment.depthClearValue = info.clearDepth;
        depthAttachment.stencilLoadOp = info.loadDepth ? wgpu::LoadOp::Load : wgpu::LoadOp::Clear;
        depthAttachment.stencilStoreOp = wgpu::StoreOp::Store;
        depthAttachment.stencilClearValue = info.clearStencil;
    }

    wgpu::RenderPassDescriptor passDesc{};
    passDesc.colorAttachmentCount = 1;
    passDesc.colorAttachments = &colorAttachment;
    if (hasDepth) {
        passDesc.depthStencilAttachment = &depthAttachment;
    }

    pass.mPass = pass.mEncoder.BeginRenderPass(&passDesc);
    if (!pass.mPass) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to begin render pass");
    }

    return pass;
}

result<RenderPass> RenderPass::Begin(
    Device& device, const Frame& frame, const ClearColor& clear) noexcept {
    RenderPassInfo info{};
    info.colorView = frame.colorView;
    info.depthView = frame.depthView;
    info.clearColor = clear;
    return Begin(device, info);
}

void RenderPass::SetPipeline(const RenderPipeline& pipeline) {
    mPass.SetPipeline(pipeline.GetHandle());
}

void RenderPass::SetBindGroup(u32 index, const BindGroup& group) {
    mPass.SetBindGroup(index, group.GetHandle());
}

void RenderPass::SetVertexBuffer(u32 slot, const Buffer& buffer, u64 offset) {
    mPass.SetVertexBuffer(slot, buffer.GetHandle(), offset);
}

void RenderPass::SetIndexBuffer(const Buffer& buffer, IndexFormat format, u64 offset) {
    mPass.SetIndexBuffer(buffer.GetHandle(), ToWGPU(format), offset);
}

void RenderPass::Draw(u32 vertexCount, u32 instanceCount, u32 firstVertex, u32 firstInstance) {
    mPass.Draw(vertexCount, instanceCount, firstVertex, firstInstance);
}

void RenderPass::DrawIndexed(u32 indexCount, u32 instanceCount,
    u32 firstIndex, i32 baseVertex, u32 firstInstance) {
    mPass.DrawIndexed(indexCount, instanceCount, firstIndex, baseVertex, firstInstance);
}

void RenderPass::SetViewport(f32 x, f32 y, f32 width, f32 height, f32 minDepth, f32 maxDepth) {
    mPass.SetViewport(x, y, width, height, minDepth, maxDepth);
}

void RenderPass::SetScissor(u32 x, u32 y, u32 width, u32 height) {
    mPass.SetScissorRect(x, y, width, height);
}

void RenderPass::Submit() {
    if (mSubmitted) return;
    mSubmitted = true;

    mPass.End();
    auto commands = mEncoder.Finish();
    mDevice->GetQueue().Submit(1, &commands);
}

} // namespace ct
