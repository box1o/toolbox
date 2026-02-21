#include "toolbox/gfx/api/swapchain.hpp"

#include <webgpu/webgpu_cpp.h>

namespace ct::gfx {

class WGPUSwapchainImpl final : public Swapchain {
public:
    WGPUSwapchainImpl();
    ~WGPUSwapchainImpl() override;

    [[nodiscard]] virtual void* GetSurface() const noexcept override { return (void*)&mSurface; }

    [[nodiscard]] Frame AcquireNextFrame() noexcept override;
    void Present() noexcept override;

public:
    bool ConfigureSwapchain(const wgpu::TextureFormat& format, const PresentMode& presentMode,
        u32 width, u32 height) noexcept;

    bool CreateSurface(ref<Device> device, ref<Window> window) noexcept;

private:
    wgpu::Surface mSurface{nullptr};
};

namespace detail {
[[nodiscard]] wgpu::Surface CreateWindowNativeSurface(ref<Device> device, ref<Window> window);
} // namespace detail

} // namespace ct::gfx
