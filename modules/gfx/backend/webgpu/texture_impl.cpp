#include "texture_impl.hpp"
#include "common.hpp"
#include "toolbox/gfx/api/device.hpp"

namespace ct::gfx::webgpu {

TextureImpl::TextureImpl(ref<Device> device, const TextureDesc& desc)
    : mDesc(desc), mDevice(std::move(device)) {

    mWidth = desc.width;
    mHeight = desc.height;
}

// NOTE: Initialize
result<void> TextureImpl::Initialize() noexcept {
    if (!mDevice) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Texture: device null");
    }

    auto device = static_cast<wgpu::Device*>(mDevice->GetNativeDeviceHandle());
    if (!device) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "Texture: Filed to aquire the device handle");
    }

    auto queue = static_cast<wgpu::Queue*>(mDevice->GetNativeQueueHandle());
    if (!queue) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "Queue: Filed to aquire the device queue handle");
    }

    wgpu::TextureDescriptor td{};
    td.dimension = wgpu::TextureDimension::e2D;
    td.size = {mWidth, mHeight, 1};
    td.mipLevelCount = mDesc.mipLevels;
    td.sampleCount = 1;
    td.format = detail::ToWGPU(mDesc.format);
    td.usage = detail::ToWGPUUsage(mDesc.usage);

    mTexture = device->CreateTexture(&td);
    if (!mTexture) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Texture: CreateTexture failed");
    }

    return ok();
}

result<ref<TextureView>> TextureImpl::GetDefaultView() noexcept {

    if (!mTexture) {
        return err(ErrorCode::INVALID_STATE, "Texture: not initialized");
    }

    auto view = mTexture.CreateView();
    if (!view) {
        return err(
            ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Texture: failed to create default view");
    }

    return ok(createRef<TextureViewImpl>(view));
};
result<ref<TextureView>> TextureImpl::CreateView(const TextureViewDesc& desc) noexcept {

    if (!mTexture) {
        return err(ErrorCode::INVALID_STATE, "Texture: not initialized");
    }

    wgpu::TextureViewDescriptor tvd{};
    tvd.baseMipLevel = desc.baseMipLevel;
    tvd.mipLevelCount =
        desc.mipLevelCount == 0 ? mDesc.mipLevels - desc.baseMipLevel : desc.mipLevelCount;

    auto view = mTexture.CreateView(&tvd);
    if (!view) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Texture: failed to create view");
    }

    return ok(createRef<TextureViewImpl>(view));
};

result<void> TextureImpl::Update(const void* data, u64 numBytes) noexcept {
    if (!mTexture) {
        return err(ErrorCode::INVALID_STATE, "Texture: not initialized");
    }
    if (!data) {
        return err(ErrorCode::INVALID_ARGUMENT, "Texture: data is null");
    }
    if (numBytes == 0) {
        return err(ErrorCode::INVALID_ARGUMENT, "Texture: numBytes is 0");
    }
    if (!HasFlag(mDesc.usage, TextureUsageFlags::CopyDst)) {
        return err(ErrorCode::INVALID_STATE, "Texture: texture missing CopyDst usage");
    }

    auto queue = static_cast<wgpu::Queue*>(mDevice->GetNativeQueueHandle());
    if (!queue || !*queue) {
        return err(ErrorCode::INVALID_STATE, "Texture: failed to acquire queue handle");
    }

    // For now, uploads are supported for 4-byte-per-pixel color formats.
    u32 bytesPerPixel = 0;
    switch (mDesc.format) {
    case TextureFormat::RGBA8Unorm:
    case TextureFormat::RGBA8UnormSrgb:
    case TextureFormat::BGRA8Unorm:
    case TextureFormat::BGRA8UnormSrgb:
        bytesPerPixel = 4;
        break;
    default:
        return err(ErrorCode::INVALID_ARGUMENT,
            "Texture: Update currently supports RGBA8/BGRA8 formats only");
    }

    const u32 unalignedBpr = mWidth * bytesPerPixel;
    const u32 alignedBpr = (unalignedBpr + 255u) & ~255u;
    const u64 requiredBytes = static_cast<u64>(alignedBpr) * static_cast<u64>(mHeight);
    if (numBytes < requiredBytes) {
        return err(ErrorCode::OUT_OF_RANGE, "Texture: upload buffer is too small");
    }

    wgpu::TexelCopyTextureInfo dst{};
    dst.texture = mTexture;
    dst.mipLevel = 0;
    dst.origin = {0, 0, 0};
    dst.aspect = wgpu::TextureAspect::All;

    wgpu::TexelCopyBufferLayout srcLayout{};
    srcLayout.offset = 0;
    srcLayout.bytesPerRow = alignedBpr;
    srcLayout.rowsPerImage = mHeight;

    wgpu::Extent3D copySize{mWidth, mHeight, 1};
    queue->WriteTexture(&dst, data, static_cast<size_t>(requiredBytes), &srcLayout, &copySize);
    return ok();
}

} // namespace ct::gfx::webgpu
