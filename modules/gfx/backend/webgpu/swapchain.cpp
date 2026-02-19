#include "swapchain.hpp"

namespace ct::gfx {

WGPUSwapchainImpl::WGPUSwapchainImpl(ref<Surface> surface, ref<Device> device) {};
WGPUSwapchainImpl::~WGPUSwapchainImpl() {};

[[nodiscard]] Frame WGPUSwapchainImpl::AcquireNextFrame() noexcept { return {}; };

void WGPUSwapchainImpl::Present() noexcept {};

bool WGPUSwapchainImpl::ConfigureSwapchain(const wgpu::TextureFormat& format,
    const PresentMode& presentMode, u32 width, u32 height) noexcept {

    return false;
}


} // namespace ct::gfx
