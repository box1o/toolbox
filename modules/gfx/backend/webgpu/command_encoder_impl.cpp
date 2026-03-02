#include "command_encoder_impl.hpp"
#include "command_buffer_impl.hpp"
#include "render_pass_encoder_impl.hpp"
#include "common.hpp"
#include "toolbox/gfx/api/device.hpp"
#include "toolbox/gfx/api/buffer.hpp"
#include "toolbox/gfx/api/texture.hpp"

namespace ct::gfx::webgpu {

CommandEncoderImpl::CommandEncoderImpl(ref<Device> device, const CommandEncoderDesc& desc)
    : mDesc(desc), mDevice(std::move(device)) {}

result<void> CommandEncoderImpl::Initialize() noexcept {
    if (!mDevice) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "CommandEncoder: device is null");
    }

    auto* nativeDevice = static_cast<wgpu::Device*>(mDevice->GetNativeDeviceHandle());
    if (!nativeDevice || !*nativeDevice) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "CommandEncoder: failed to acquire device handle");
    }

    wgpu::CommandEncoderDescriptor ced{};
    ced.label = wgpu::StringView{mDesc.debugName.c_str(), mDesc.debugName.size()};

    mEncoder = nativeDevice->CreateCommandEncoder(&ced);
    if (!mEncoder) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "CommandEncoder: CreateCommandEncoder failed");
    }

    return ok();
}

result<ref<RenderPassEncoder>> CommandEncoderImpl::BeginRenderPass(
    const RenderPassDesc& desc) noexcept {
    if (!mEncoder) {
        return err(ErrorCode::INVALID_STATE, "CommandEncoder: encoder is null");
    }
    if (mFinished) {
        return err(ErrorCode::INVALID_STATE, "CommandEncoder: already finished");
    }

    //NOTE: Color attachments
    std::vector<wgpu::RenderPassColorAttachment> colorAttachments;
    colorAttachments.reserve(desc.colors.size());

    for (const auto& color : desc.colors) {
        if (!color.view) {
            return err(ErrorCode::INVALID_ARGUMENT,
                "CommandEncoder: color attachment view is null");
        }

        wgpu::RenderPassColorAttachment ca{};
        ca.view = *static_cast<wgpu::TextureView*>(color.view);
        ca.loadOp = color.clearEnabled ? wgpu::LoadOp::Clear : wgpu::LoadOp::Load;
        ca.storeOp = wgpu::StoreOp::Store;
        ca.clearValue = wgpu::Color{
            static_cast<double>(color.clear.r),
            static_cast<double>(color.clear.g),
            static_cast<double>(color.clear.b),
            static_cast<double>(color.clear.a),
        };
        colorAttachments.push_back(ca);
    }

    //NOTE: Depth stencil attachment
    wgpu::RenderPassDepthStencilAttachment depthAttachment{};
    bool useDepth = desc.enableDepth && desc.depthView;

    if (useDepth) {
        depthAttachment.view = *static_cast<wgpu::TextureView*>(desc.depthView);
        depthAttachment.depthLoadOp = wgpu::LoadOp::Clear;
        depthAttachment.depthStoreOp = wgpu::StoreOp::Store;
        depthAttachment.depthClearValue = desc.clearDepth;
        depthAttachment.stencilLoadOp = wgpu::LoadOp::Clear;
        depthAttachment.stencilStoreOp = wgpu::StoreOp::Store;
        depthAttachment.stencilClearValue = desc.clearStencil;
    }

    wgpu::RenderPassDescriptor rpd{};
    rpd.label = wgpu::StringView{desc.debugName.c_str(), desc.debugName.size()};
    rpd.colorAttachmentCount = colorAttachments.size();
    rpd.colorAttachments = colorAttachments.data();
    rpd.depthStencilAttachment = useDepth ? &depthAttachment : nullptr;

    auto nativePass = mEncoder.BeginRenderPass(&rpd);
    if (!nativePass) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "CommandEncoder: BeginRenderPass failed");
    }

    return ok(createRef<RenderPassEncoderImpl>(std::move(nativePass)));
}

result<void> CommandEncoderImpl::CopyBufferToBuffer(
    ref<Buffer> src, u64 srcOffset,
    ref<Buffer> dst, u64 dstOffset,
    u64 size) noexcept {
    if (!mEncoder) return err(ErrorCode::INVALID_STATE, "CommandEncoder: encoder is null");
    if (mFinished)  return err(ErrorCode::INVALID_STATE, "CommandEncoder: already finished");
    if (!src)       return err(ErrorCode::INVALID_ARGUMENT, "CopyBufferToBuffer: src is null");
    if (!dst)       return err(ErrorCode::INVALID_ARGUMENT, "CopyBufferToBuffer: dst is null");

    auto* nativeSrc = static_cast<wgpu::Buffer*>(src->GetNativeBufferHandle());
    auto* nativeDst = static_cast<wgpu::Buffer*>(dst->GetNativeBufferHandle());
    if (!nativeSrc || !*nativeSrc) return err(ErrorCode::INVALID_STATE, "CopyBufferToBuffer: src handle is null");
    if (!nativeDst || !*nativeDst) return err(ErrorCode::INVALID_STATE, "CopyBufferToBuffer: dst handle is null");

    if (size == 0) size = src->GetSize() - srcOffset;

    if (srcOffset + size > src->GetSize()) {
        return err(ErrorCode::OUT_OF_RANGE, "CopyBufferToBuffer: src range exceeds buffer size");
    }
    if (dstOffset + size > dst->GetSize()) {
        return err(ErrorCode::OUT_OF_RANGE, "CopyBufferToBuffer: dst range exceeds buffer size");
    }

    mEncoder.CopyBufferToBuffer(*nativeSrc, srcOffset, *nativeDst, dstOffset, size);
    return ok();
}

result<void> CommandEncoderImpl::ClearBuffer(ref<Buffer> buffer, u64 offset, u64 size) noexcept {
    if (!mEncoder) return err(ErrorCode::INVALID_STATE, "CommandEncoder: encoder is null");
    if (mFinished)  return err(ErrorCode::INVALID_STATE, "CommandEncoder: already finished");
    if (!buffer)    return err(ErrorCode::INVALID_ARGUMENT, "ClearBuffer: buffer is null");

    auto* nativeBuffer = static_cast<wgpu::Buffer*>(buffer->GetNativeBufferHandle());
    if (!nativeBuffer || !*nativeBuffer) {
        return err(ErrorCode::INVALID_STATE, "ClearBuffer: buffer handle is null");
    }

    //NOTE: size=0 means whole buffer from offset
    u64 effectiveSize = (size == 0) ? (buffer->GetSize() - offset) : size;

    if (offset + effectiveSize > buffer->GetSize()) {
        return err(ErrorCode::OUT_OF_RANGE, "ClearBuffer: range exceeds buffer size");
    }

    mEncoder.ClearBuffer(*nativeBuffer, offset, effectiveSize);
    return ok();
}

result<void> CommandEncoderImpl::CopyBufferToTexture(
    const BufferCopyView& src,
    const TextureCopyView& dst,
    const Extent3D& copySize) noexcept {
    if (!mEncoder) return err(ErrorCode::INVALID_STATE, "CommandEncoder: encoder is null");
    if (mFinished)  return err(ErrorCode::INVALID_STATE, "CommandEncoder: already finished");
    if (!src.buffer) return err(ErrorCode::INVALID_ARGUMENT, "CopyBufferToTexture: src buffer is null");
    if (!dst.texture) return err(ErrorCode::INVALID_ARGUMENT, "CopyBufferToTexture: dst texture is null");

    auto* nativeBuffer = static_cast<wgpu::Buffer*>(src.buffer->GetNativeBufferHandle());
    auto* nativeTexture = static_cast<wgpu::Texture*>(dst.texture->GetNativeTextureHandle());
    if (!nativeBuffer || !*nativeBuffer) return err(ErrorCode::INVALID_STATE, "CopyBufferToTexture: buffer handle is null");
    if (!nativeTexture || !*nativeTexture) return err(ErrorCode::INVALID_STATE, "CopyBufferToTexture: texture handle is null");

    wgpu::TexelCopyBufferInfo bufInfo{};
    bufInfo.buffer = *nativeBuffer;
    // bufInfo.offset = src.offset;
    // bufInfo.bytesPerRow = src.bytesPerRow;
    // bufInfo.rowsPerImage = src.rowsPerImage;
    //
    wgpu::TexelCopyTextureInfo texInfo{};
    texInfo.texture = *nativeTexture;
    texInfo.mipLevel = dst.mipLevel;
    texInfo.origin = {dst.originX, dst.originY, dst.originZ};

    wgpu::Extent3D extent{copySize.width, copySize.height, copySize.depthOrArrayLayers};

    mEncoder.CopyBufferToTexture(&bufInfo, &texInfo, &extent);
    return ok();
}

result<void> CommandEncoderImpl::CopyTextureToBuffer(
    const TextureCopyView& src,
    const BufferCopyView& dst,
    const Extent3D& copySize) noexcept {
    if (!mEncoder) return err(ErrorCode::INVALID_STATE, "CommandEncoder: encoder is null");
    if (mFinished)  return err(ErrorCode::INVALID_STATE, "CommandEncoder: already finished");
    if (!src.texture) return err(ErrorCode::INVALID_ARGUMENT, "CopyTextureToBuffer: src texture is null");
    if (!dst.buffer) return err(ErrorCode::INVALID_ARGUMENT, "CopyTextureToBuffer: dst buffer is null");

    auto* nativeTexture = static_cast<wgpu::Texture*>(src.texture->GetNativeTextureHandle());
    auto* nativeBuffer = static_cast<wgpu::Buffer*>(dst.buffer->GetNativeBufferHandle());
    if (!nativeTexture || !*nativeTexture) return err(ErrorCode::INVALID_STATE, "CopyTextureToBuffer: texture handle is null");
    if (!nativeBuffer || !*nativeBuffer) return err(ErrorCode::INVALID_STATE, "CopyTextureToBuffer: buffer handle is null");

    wgpu::TexelCopyTextureInfo texInfo{};
    texInfo.texture = *nativeTexture;
    texInfo.mipLevel = src.mipLevel;
    texInfo.origin = {src.originX, src.originY, src.originZ};

    wgpu::TexelCopyBufferInfo bufInfo{};
    bufInfo.buffer = *nativeBuffer;
    // bufInfo.offset = dst.offset;
    // bufInfo.bytesPerRow = dst.bytesPerRow;
    // bufInfo.rowsPerImage = dst.rowsPerImage;

    wgpu::Extent3D extent{copySize.width, copySize.height, copySize.depthOrArrayLayers};

    mEncoder.CopyTextureToBuffer(&texInfo, &bufInfo, &extent);
    return ok();
}

result<void> CommandEncoderImpl::CopyTextureToTexture(
    const TextureCopyView& src,
    const TextureCopyView& dst,
    const Extent3D& copySize) noexcept {
    if (!mEncoder) return err(ErrorCode::INVALID_STATE, "CommandEncoder: encoder is null");
    if (mFinished)  return err(ErrorCode::INVALID_STATE, "CommandEncoder: already finished");
    if (!src.texture) return err(ErrorCode::INVALID_ARGUMENT, "CopyTextureToTexture: src texture is null");
    if (!dst.texture) return err(ErrorCode::INVALID_ARGUMENT, "CopyTextureToTexture: dst texture is null");

    auto* nativeSrc = static_cast<wgpu::Texture*>(src.texture->GetNativeTextureHandle());
    auto* nativeDst = static_cast<wgpu::Texture*>(dst.texture->GetNativeTextureHandle());
    if (!nativeSrc || !*nativeSrc) return err(ErrorCode::INVALID_STATE, "CopyTextureToTexture: src handle is null");
    if (!nativeDst || !*nativeDst) return err(ErrorCode::INVALID_STATE, "CopyTextureToTexture: dst handle is null");

    wgpu::TexelCopyTextureInfo srcInfo{};
    srcInfo.texture = *nativeSrc;
    srcInfo.mipLevel = src.mipLevel;
    srcInfo.origin = {src.originX, src.originY, src.originZ};

    wgpu::TexelCopyTextureInfo dstInfo{};
    dstInfo.texture = *nativeDst;
    dstInfo.mipLevel = dst.mipLevel;
    dstInfo.origin = {dst.originX, dst.originY, dst.originZ};

    wgpu::Extent3D extent{copySize.width, copySize.height, copySize.depthOrArrayLayers};

    mEncoder.CopyTextureToTexture(&srcInfo, &dstInfo, &extent);
    return ok();
}

result<ref<CommandBuffer>> CommandEncoderImpl::Finish(const CommandBufferDesc& desc) noexcept {
    if (!mEncoder) {
        return err(ErrorCode::INVALID_STATE, "CommandEncoder: encoder is null");
    }
    if (mFinished) {
        return err(ErrorCode::INVALID_STATE, "CommandEncoder: already finished");
    }

    wgpu::CommandBufferDescriptor cbd{};
    cbd.label = wgpu::StringView{desc.debugName.c_str(), desc.debugName.size()};

    auto nativeCmdBuffer = mEncoder.Finish(&cbd);
    if (!nativeCmdBuffer) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "CommandEncoder: Finish failed");
    }

    mFinished = true;
    return ok(createRef<CommandBufferImpl>(std::move(nativeCmdBuffer), desc));
}

} // namespace ct::gfx::webgpu
