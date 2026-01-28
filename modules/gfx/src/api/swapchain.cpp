#include "ct/gfx/api/swapchain.hpp"

namespace ct::gfx {

result<scope<Swapchain>> Swapchain::Create(weak<Window> window, weak<Device> device,const SwapchainInfo& info) {
    auto dev = device.lock();
    auto win = window.lock();

    // switch (info.backend) {
    //     case DeviceBackend::Vulkan:
    //         return vk::VKDeviceImpl::Create(info);
    //     case DeviceBackend::Metal:
    //         return err( ErrorCode::GRAPHICS_UNSUPPORTED_API, "Metal backend is not implemented yet");
    //     default:
    //         return err( ErrorCode::GRAPHICS_UNSUPPORTED_API, "Unsupported graphics backend");
    // }
    return err( ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Unsupported graphics backend");
}

}
