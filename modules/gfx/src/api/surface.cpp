#include "toolbox/gfx/api/surface.hpp"
#include "toolbox/gfx/api/device.hpp"
#include "toolbox/gfx/window/window.hpp"

namespace ct {

namespace {

wgpu::PresentMode ToWGPU(PresentMode mode) {
    switch (mode) {
    case PresentMode::Immediate: return wgpu::PresentMode::Immediate;
    case PresentMode::VSync:     return wgpu::PresentMode::Fifo;
    case PresentMode::Mailbox:   return wgpu::PresentMode::Mailbox;
    }
    return wgpu::PresentMode::Fifo;
}

wgpu::TextureFormat SelectFormat(
    [[maybe_unused]] const wgpu::Surface& surface,
    [[maybe_unused]] const wgpu::Adapter& adapter) {
#ifdef WEBGPU_BACKEND_EMSCRIPTEN
    //NOTE: Browser prefers BGRA8Unorm, avoids extra copy
    return wgpu::TextureFormat::BGRA8Unorm;
#else
    wgpu::SurfaceCapabilities caps;
    surface.GetCapabilities(adapter, &caps);

    for (size_t i = 0; i < caps.formatCount; ++i) {
        if (caps.formats[i] == wgpu::TextureFormat::BGRA8Unorm)
            return wgpu::TextureFormat::BGRA8Unorm;
    }
    for (size_t i = 0; i < caps.formatCount; ++i) {
        if (caps.formats[i] == wgpu::TextureFormat::RGBA8Unorm)
            return wgpu::TextureFormat::RGBA8Unorm;
    }
    if (caps.formatCount > 0) return caps.formats[0];
    return wgpu::TextureFormat::BGRA8Unorm;
#endif
}

wgpu::PresentMode SelectPresentMode(
    PresentMode requested,
    [[maybe_unused]] const wgpu::Surface& surface,
    [[maybe_unused]] const wgpu::Adapter& adapter) {
#ifdef WEBGPU_BACKEND_EMSCRIPTEN
    (void)requested;
    return wgpu::PresentMode::Fifo;
#else
    wgpu::SurfaceCapabilities caps;
    surface.GetCapabilities(adapter, &caps);

    wgpu::PresentMode wanted = ToWGPU(requested);
    for (size_t i = 0; i < caps.presentModeCount; ++i) {
        if (caps.presentModes[i] == wanted) return wanted;
    }
    return wgpu::PresentMode::Fifo;
#endif
}

} // namespace

result<ref<Surface>> Surface::Create(
    ref<Window> window, ref<Device> device, const SurfaceInfo& info) noexcept {

    if (!window) return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Window is null");
    if (!device) return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Device is null");

    ref<Surface> surface(new Surface());
    surface->mDevice = device;
    surface->mWindow = window;
    surface->mPresentMode = info.presentMode;
    surface->mDepthEnabled = info.enableDepth;
    surface->mDepthFormat = info.depthFormat;

    if (!surface->CreateNativeSurface(*window, *device)) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to create native surface");
    }

    u32 w = window->GetWidth();
    u32 h = window->GetHeight();

    if (!surface->Configure(*device, w, h)) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to configure surface");
    }

    if (info.enableDepth) {
        if (!surface->CreateDepthTexture(*device, w, h)) {
            return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to create depth texture");
        }
    }

    window->SetResizeCallback([weak = weak<Surface>(surface)](u32 width, u32 height) {
        if (auto s = weak.lock()) {
            s->Resize(width, height);
        }
    });

    log::Info("[wgpu] Surface created ({}x{})", w, h);
    return surface;
}

Surface::~Surface() {
    mDepthView = nullptr;
    if (mDepthTexture) {
        mDepthTexture.Destroy();
        mDepthTexture = nullptr;
    }
    mSurface = nullptr;
}

bool Surface::CreateNativeSurface(const Window& window, const Device& device) {
    mSurface = detail::CreateWindowNativeSurface(device.GetInstance(), window);
    if (!mSurface) {
        log::Critical("Failed to create native surface");
        return false;
    }
    return true;
}

bool Surface::Configure(const Device& device, u32 width, u32 height) {
    if (width == 0 || height == 0) return true;

    mFormat = SelectFormat(mSurface, device.GetAdapter());
    wgpu::PresentMode presentMode = SelectPresentMode(mPresentMode, mSurface, device.GetAdapter());

    wgpu::SurfaceConfiguration config{};
    config.device = device.GetDevice();
    config.format = mFormat;
    config.width = width;
    config.height = height;
    config.presentMode = presentMode;
    config.alphaMode = wgpu::CompositeAlphaMode::Auto;
    config.usage = wgpu::TextureUsage::RenderAttachment;

    mSurface.Configure(&config);

    mWidth = width;
    mHeight = height;
    return true;
}

bool Surface::CreateDepthTexture(const Device& device, u32 width, u32 height) {
    if (width == 0 || height == 0) return true;

    mDepthView = nullptr;
    if (mDepthTexture) {
        mDepthTexture.Destroy();
        mDepthTexture = nullptr;
    }

    wgpu::TextureDescriptor desc{};
    desc.size = {width, height, 1};
    desc.mipLevelCount = 1;
    desc.sampleCount = 1;
    desc.dimension = wgpu::TextureDimension::e2D;
    desc.format = ct::ToWGPU(mDepthFormat);
    desc.usage = wgpu::TextureUsage::RenderAttachment;

    mDepthTexture = device.GetDevice().CreateTexture(&desc);
    if (!mDepthTexture) return false;

    wgpu::TextureViewDescriptor viewDesc{};
    viewDesc.format = ct::ToWGPU(mDepthFormat);
    viewDesc.dimension = wgpu::TextureViewDimension::e2D;
    viewDesc.mipLevelCount = 1;
    viewDesc.arrayLayerCount = 1;

    mDepthView = mDepthTexture.CreateView(&viewDesc);
    return mDepthView != nullptr;
}

result<Frame> Surface::BeginFrame() noexcept {
    wgpu::SurfaceTexture surfaceTexture;
    mSurface.GetCurrentTexture(&surfaceTexture);

#ifndef WEBGPU_BACKEND_EMSCRIPTEN
    if (surfaceTexture.status == wgpu::SurfaceGetCurrentTextureStatus::Outdated ||
        surfaceTexture.status == wgpu::SurfaceGetCurrentTextureStatus::Lost) {
        auto dev = mDevice.lock();
        if (dev) {
            Configure(*dev, mWidth, mHeight);
            if (mDepthEnabled) CreateDepthTexture(*dev, mWidth, mHeight);
            mSurface.GetCurrentTexture(&surfaceTexture);
        }
    }

    if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal &&
        surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessSuboptimal) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to acquire surface texture");
    }
#endif

    wgpu::TextureView colorView = surfaceTexture.texture.CreateView();
    if (!colorView) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to create color view");
    }

    return Frame{
        .colorView = colorView,
        .depthView = mDepthView,
        .width = mWidth,
        .height = mHeight,
    };
}

void Surface::Present() noexcept {
#ifndef WEBGPU_BACKEND_EMSCRIPTEN
    //NOTE: Browser composites automatically, calling Present() crashes on Emscripten
    mSurface.Present();
#endif
    auto dev = mDevice.lock();
    if (dev) dev->Tick();
}

void Surface::Resize(u32 width, u32 height) {
    if (width == 0 || height == 0) return;
    if (width == mWidth && height == mHeight) return;

    auto dev = mDevice.lock();
    if (!dev) return;

    Configure(*dev, width, height);
    if (mDepthEnabled) CreateDepthTexture(*dev, width, height);

    log::Info("[wgpu] Surface resized ({}x{})", width, height);
}

TextureFormat Surface::GetFormat() const noexcept {
    if (mFormat == wgpu::TextureFormat::BGRA8Unorm) return TextureFormat::BGRA8Unorm;
    if (mFormat == wgpu::TextureFormat::RGBA8Unorm) return TextureFormat::RGBA8Unorm;
    return TextureFormat::BGRA8Unorm;
}

TextureFormat Surface::GetDepthFormat() const noexcept { return mDepthFormat; }
bool Surface::HasDepth() const noexcept { return mDepthEnabled; }
u32 Surface::GetWidth() const noexcept { return mWidth; }
u32 Surface::GetHeight() const noexcept { return mHeight; }

f32 Surface::GetAspectRatio() const noexcept {
    return (mHeight > 0) ? static_cast<f32>(mWidth) / static_cast<f32>(mHeight) : 1.0f;
}

PresentMode Surface::GetPresentMode() const noexcept { return mPresentMode; }
wgpu::Surface Surface::GetHandle() const noexcept { return mSurface; }

} // namespace ct
