#pragma once
#include "toolbox/gfx/api/swapchain.hpp"

#include <webgpu/webgpu_cpp.h>

namespace ct::gfx::webgpu {

class SwapchainImpl final : public Swapchain {
public:
    explicit SwapchainImpl(ref<Device> device, ref<Window> window, const SwapchainDesc& desc = {});
    ~SwapchainImpl() override = default;

    [[nodiscard]] TextureFormat GetColorFormat() const noexcept override {
        return mDesc.colorFormat;
    };
    [[nodiscard]] TextureFormat GetDepthFormat() const noexcept override {
        return mDesc.depthFormat;
    };
    // [[nodiscard]]  bool HasDepth() const noexcept override;
    // [[nodiscard]]  u32 GetWidth() const noexcept override;
    // [[nodiscard]]  u32 GetHeight() const noexcept override;
    // [[nodiscard]]  f32 GetAspectRatio() const noexcept override;

    // clang-format off
    [[nodiscard]] void* GetNativeSurfaceHandle() noexcept override { return static_cast<void*>(&mSurface); };
    // clang-format on

    void Resize(u32 width, u32 height) noexcept override;
    [[nodiscard]] result<Frame> AcquireNextFrame() noexcept override;
    result<void> Present() noexcept override;

    // NOTE: Initialize
    result<void> Initialize() noexcept override;

private:
    result<void> Configure() noexcept;
    result<void> CreateOrResizeDepth() noexcept;

private:
    SwapchainDesc mDesc{};

    ref<Device> mDevice{nullptr};
    ref<Window> mWindow{nullptr};

    wgpu::Surface mSurface;

    u32 mWidth{0};
    u32 mHeight{0};


    wgpu::SurfaceTexture mCurrent{};
    wgpu::TextureView mCurrentView{nullptr};
    wgpu::Texture mDepthTex{nullptr};
    wgpu::TextureView mDepthView{nullptr};
};

[[nodiscard]] wgpu::Surface CreateNativeSurface(const wgpu::Instance& instance, ref<Window> window);

} // namespace ct::gfx::webgpu
