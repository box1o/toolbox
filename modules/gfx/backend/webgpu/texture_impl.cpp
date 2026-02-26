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
    td.format = ToWGPU(mDesc.format);
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

} // namespace ct::gfx::webgpu
