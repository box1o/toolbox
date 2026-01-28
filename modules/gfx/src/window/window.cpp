#include "ct/gfx/window/window.hpp"
#include "ct/base/types/types.hpp"
#include  "../../backends/glfw/glfw_window.hpp"

namespace ct::gfx {

ref<Window> Window::Create(const WindowInfo& info) {
    return createRef<GLFWwindowImpl>( info );
    // switch (info.backend) {
    //     case DeviceBackend::Vulkan:
    //         return vk::VKDeviceImpl::Create(info);
    //     case DeviceBackend::Metal:
    //         return err( ErrorCode::GRAPHICS_UNSUPPORTED_API, "Metal backend is not implemented yet");
    //     default:
    //         return err( ErrorCode::GRAPHICS_UNSUPPORTED_API, "Unsupported graphics backend");
    // }
}

}
