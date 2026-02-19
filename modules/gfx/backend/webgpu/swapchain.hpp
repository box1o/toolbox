#include "toolbox/gfx/api/swapchain.hpp"

#include <webgpu/webgpu_cpp.h>

namespace ct::gfx {

class WGPUSwapchainImpl final : public Swapchain {
public:
    WGPUSwapchainImpl(ref<Surface> surface, ref<Device> device);
    ~WGPUSwapchainImpl() override;


    [[nodiscard]] Frame AcquireNextFrame() noexcept override;
    void Present() noexcept override;

public:
    bool ConfigureSwapchain(const wgpu::TextureFormat& format, const PresentMode& presentMode,
        u32 width, u32 height) noexcept;

private:
    ref<wgpu::Surface> mSurface{nullptr};

    // wgpu::Surface mSurface{nullptr};
    // wgpu::TextureFormat mFormat{wgpu::TextureFormat::BGRA8Unorm};
    // PresentMode mPresentMode{PresentMode::VSync};
    // u32 mWidth{0};
    // u32 mHeight{0};
    //
    // bool mDepthEnabled{false};
    // TextureFormat mDepthFormat{TextureFormat::Depth24PlusStencil8};
    // wgpu::Texture mDepthTexture{nullptr};
    // wgpu::TextureView mDepthView{nullptr};
};

} // namespace ct::gfx
