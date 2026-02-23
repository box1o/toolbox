#pragma once
#include <toolbox/gfx/api/swapchain.hpp>

#include <webgpu/webgpu_cpp.h>

namespace ct::gfx {
class Device;
class Surface;
} // namespace ct::gfx

namespace ct::gfx::webgpu {

class SwapchainImpl final : public Swapchain {
public:
    SwapchainImpl() = default;
    ~SwapchainImpl() override = default;

    result<void> Initialize(
        ref<Device> device, ref<Surface> surface, const SwapchainDesc& desc) noexcept;

    [[nodiscard]] TextureFormat GetColorFormat() const noexcept override { return mColorFormat; }
    [[nodiscard]] TextureFormat GetDepthFormat() const noexcept override { return mDepthFormat; }
    [[nodiscard]] bool HasDepth() const noexcept override { return mHasDepth; }

    [[nodiscard]] u32 GetWidth() const noexcept override { return mWidth; }
    [[nodiscard]] u32 GetHeight() const noexcept override { return mHeight; }
    [[nodiscard]] f32 GetAspectRatio() const noexcept override {
        return (mHeight > 0) ? (f32)mWidth / (f32)mHeight : 1.0f;
    }

    void Resize(u32 width, u32 height) noexcept override;

    [[nodiscard]] result<Frame> AcquireNextFrame() noexcept override;
    result<void> Present() noexcept override;

private:
    result<void> Configure() noexcept;
    result<void> CreateOrResizeDepth() noexcept;

private:
    ref<Device> mDeviceRef{};
    wgpu::Device mDevice;
    wgpu::Queue mQueue;
    wgpu::Surface mSurface;

    SwapchainDesc mDesc{};
    TextureFormat mColorFormat{TextureFormat::Undefined};
    TextureFormat mDepthFormat{TextureFormat::Undefined};
    bool mHasDepth{false};

    u32 mWidth{0};
    u32 mHeight{0};

    wgpu::SurfaceTexture mCurrent{};

    // C++ wrapper views owned by swapchain; Frame carries pointers to these.
    wgpu::TextureView mCurrentView{nullptr};

    wgpu::Texture mDepthTex{nullptr};
    wgpu::TextureView mDepthView{nullptr};
};

} // namespace ct::gfx::webgpu
