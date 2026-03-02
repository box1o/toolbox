#include "swapchain_impl.hpp"
#include "common.hpp"

#include "toolbox/base/logger/logger.hpp"
#include "toolbox/gfx/api/device.hpp"
#include "toolbox/gfx/window/window.hpp"

#include <cassert>

namespace ct::gfx::webgpu {

SwapchainImpl::SwapchainImpl(ref<Device> device, ref<Window> window, const SwapchainDesc& desc)
    : mDesc(desc), mDevice(std::move(device)), mWindow(std::move(window)) {}

result<void> SwapchainImpl::Initialize() noexcept {
    if (!mDevice || !mWindow) {
        return err(
            ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Surface: device or window is null");
    }

    auto instance = reinterpret_cast<wgpu::Instance*>(mDevice->GetNativeInstanceHandle());
    if (!instance) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "Surface: device instance handle is null");
    }

    mSurface = CreateNativeSurface(*instance, mWindow);
    if (!mSurface) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "Surface: failed to create native surface");
    }

    mWidth = mWindow->GetWidth();
    mHeight = mWindow->GetHeight();

    TRY_RETURN(Configure());
    TRY_RETURN(CreateOrResizeDepth());

    return ok();
}

void SwapchainImpl::Resize(u32 width, u32 height) noexcept {
    if (width == mWidth && height == mHeight) return;
    if (width == 0 || height == 0) return;

    mWidth = width;
    mHeight = height;

    if (auto res = Configure(); !res) {
        log::Critical("Failed to configure");
    }

    if (auto res = CreateOrResizeDepth(); !res) {
        log::Critical("Failed to CreateOrResizeDepth");
    }
}

result<Frame> SwapchainImpl::AcquireNextFrame() noexcept {
    if (!mSurface) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Swapchain: surface null");
    }

    mCurrentView = nullptr;
    mCurrent.texture = nullptr;

    mSurface.GetCurrentTexture(&mCurrent);

    if (!mCurrent.texture) {
        return err(
            ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Swapchain: GetCurrentTexture failed");
    }

    mCurrentView = mCurrent.texture.CreateView();
    if (!mCurrentView) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Swapchain: CreateView failed");
    }

    Frame f{};
    f.colorTexture = (void*)mCurrent.texture.Get();
    f.colorView = (void*)&mCurrentView;
    f.width = mWidth;
    f.height = mHeight;

    if (mDesc.enableDepth) {
        if (!mDepthView) {
            return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Swapchain: depth view null");
        }
        f.depthTexture = (void*)mDepthTex.Get();
        f.depthView = (void*)&mDepthView;
    }

    return ok(f);
}

result<void> SwapchainImpl::Present() noexcept {
    if (!mSurface) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Swapchain: surface null");
    }

#if !defined(__EMSCRIPTEN__)
    mSurface.Present();
#endif

    mCurrentView = nullptr;
    mCurrent.texture = nullptr;

    return ok();
}

result<void> SwapchainImpl::Configure() noexcept {
    if (!mSurface || !mDevice) {
        return err(
            ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Swapchain: surface or device null");
    }

    auto device = static_cast<wgpu::Device*>(mDevice->GetNativeDeviceHandle());
    if (!device) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "Swapchain: failed to acquire device handle");
    }

    wgpu::SurfaceConfiguration cfg{};
    cfg.device = *device;
    cfg.format = detail::ToWGPU(mDesc.colorFormat);
    cfg.usage = wgpu::TextureUsage::RenderAttachment;
    cfg.width = mWidth;
    cfg.height = mHeight;
    cfg.presentMode = detail::ToWGPU(mDesc.presentMode);
    cfg.alphaMode = wgpu::CompositeAlphaMode::Auto;
    mSurface.Configure(&cfg);
    return ok();
}

result<void> SwapchainImpl::CreateOrResizeDepth() noexcept {
    if (!mDesc.enableDepth) return ok();
    if (!mDevice) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Swapchain: device null");
    }

    mDepthView = nullptr;
    mDepthTex = nullptr;

    wgpu::TextureDescriptor td{};
    td.dimension = wgpu::TextureDimension::e2D;
    td.size = {mWidth, mHeight, 1};
    td.mipLevelCount = 1;
    td.sampleCount = 1;
    td.format = detail::ToWGPU(mDesc.depthFormat);
    td.usage = wgpu::TextureUsage::RenderAttachment;

    auto device = static_cast<wgpu::Device*>(mDevice->GetNativeDeviceHandle());
    if (!device) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "Swapchain: failed to acquire device handle");
    }

    mDepthTex = device->CreateTexture(&td);
    if (!mDepthTex) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "Swapchain: failed to create depth texture");
    }

    mDepthView = mDepthTex.CreateView();
    if (!mDepthView) {
        mDepthTex = nullptr;
        return err(
            ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Swapchain: failed to create depth view");
    }
    return ok();
}

} // namespace ct::gfx::webgpu
