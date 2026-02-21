#include "swapchain.hpp"
#include <webgpu/webgpu_cpp.h>
#include "toolbox/gfx/api/device.hpp"

namespace ct::gfx {

WGPUSwapchainImpl::WGPUSwapchainImpl() { };
WGPUSwapchainImpl::~WGPUSwapchainImpl() {};

Frame WGPUSwapchainImpl::AcquireNextFrame() noexcept { return Frame{}; };
void WGPUSwapchainImpl::Present() noexcept {};

bool WGPUSwapchainImpl::ConfigureSwapchain(const wgpu::TextureFormat& format,
    const PresentMode& presentMode, u32 width, u32 height) noexcept {

    return true;
};




bool WGPUSwapchainImpl::CreateSurface(ref<Device> device, ref<Window> window) noexcept {

    if (!device) {
        log::Error("WGPUSurfaceImpl::CreateSurface: instance is null");
        return false;
    }
    if (!window) {
        log::Error("WGPUSurfaceImpl::CreateSurface: window is null");
        return false;
    }


    mSurface = detail::CreateWindowNativeSurface(device, window);
    if (!mSurface) {
        log::Error("Failed to create native surface");
        return false;
    }
    return true;
}

} // namespace ct::gfx
