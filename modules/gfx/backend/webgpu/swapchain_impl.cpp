#include "swapchain_impl.hpp"
#include "conversion.hpp"
#include "device_impl.hpp"
#include "surface_impl.hpp"

#include <toolbox/base/logger/logger.hpp>

namespace ct::gfx::webgpu {

bool SwapchainImpl::Init(ref<Device> device, ref<Surface> surface, const SwapchainDesc& desc) noexcept {
    if (!device || !surface) return false;

    auto* dev  = dynamic_cast<DeviceImpl*>(device.get());
    auto* surf = dynamic_cast<SurfaceImpl*>(surface.get());
    if (!dev || !surf) {
        log::Error("SwapchainImpl: invalid backend objects");
        return false;
    }

    mDeviceRef = device;
    mDevice  = dev->DeviceHandle();
    mQueue   = dev->QueueHandle();
    mSurface = surf->SurfaceHandle();

    mDesc = desc;
    mColorFormat = desc.preferredFormat;
    mHasDepth = desc.enableDepth;
    mDepthFormat = desc.depthFormat;

    mWidth = 1;
    mHeight = 1;

    if (!Configure()) return false;
    if (mHasDepth && !CreateOrResizeDepth()) return false;

    return true;
}

void SwapchainImpl::Resize(u32 width, u32 height) noexcept {
    if (width == 0 || height == 0) return;
    if (width == mWidth && height == mHeight) return;

    // Avoid resizing while holding an acquired frame
    if (mCurrent.texture) return;

    mWidth = width;
    mHeight = height;

    if (!Configure()) {
        log::Error("SwapchainImpl::Resize: configure failed");
        return;
    }
    if (mHasDepth && !CreateOrResizeDepth()) {
        log::Error("SwapchainImpl::Resize: depth recreate failed");
    }
}

bool SwapchainImpl::Configure() noexcept {
    if (!mSurface || !mDevice) return false;

    wgpu::SurfaceConfiguration cfg{};
    cfg.device = mDevice;
    cfg.format = ToWGPU(mColorFormat);
    cfg.usage  = wgpu::TextureUsage::RenderAttachment;
    cfg.width  = mWidth;
    cfg.height = mHeight;
    cfg.presentMode = ToWGPU(mDesc.presentMode);
    cfg.alphaMode   = wgpu::CompositeAlphaMode::Auto;

    mSurface.Configure(&cfg);
    return true;
}

bool SwapchainImpl::CreateOrResizeDepth() noexcept {
    if (!mHasDepth) return true;
    if (!mDevice) return false;

    mDepthView = nullptr;
    mDepthTex  = nullptr;

    wgpu::TextureDescriptor td{};
    td.dimension = wgpu::TextureDimension::e2D;
    td.size = { mWidth, mHeight, 1 };
    td.mipLevelCount = 1;
    td.sampleCount = 1;
    td.format = ToWGPU(mDepthFormat);
    td.usage = wgpu::TextureUsage::RenderAttachment;

    mDepthTex = mDevice.CreateTexture(&td);
    if (!mDepthTex) return false;

    mDepthView = mDepthTex.CreateView();
    return mDepthView != nullptr;
}

result<Frame> SwapchainImpl::AcquireNextFrame() noexcept {
    if (!mSurface) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Swapchain: surface null");
    }

    // Clear previous per-frame state
    mCurrentView = nullptr;
    mCurrent.texture = nullptr;

    mSurface.GetCurrentTexture(&mCurrent);

    if (!mCurrent.texture) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Swapchain: GetCurrentTexture failed");
    }

    mCurrentView = mCurrent.texture.CreateView();
    if (!mCurrentView) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Swapchain: CreateView failed");
    }

    Frame f{};
    f.colorTexture = (void*)mCurrent.texture.Get();
    f.colorView    = (void*)&mCurrentView; // pointer to C++ wrapper
    f.width = mWidth;
    f.height = mHeight;

    if (mHasDepth) {
        if (!mDepthView) {
            return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Swapchain: depth view null");
        }
        f.depthTexture = (void*)mDepthTex.Get();
        f.depthView    = (void*)&mDepthView; // pointer to C++ wrapper
    }

    return ok(f);
}

result<void> SwapchainImpl::Present() noexcept {
    if (!mSurface) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Swapchain: surface null");
    }

    // Present the current swapchain texture
    mSurface.Present();

    // Mark frame done
    mCurrentView = nullptr;
    mCurrent.texture = nullptr;

    return ok();
}

} // namespace ct::gfx::webgpu
