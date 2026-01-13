#include "ct/gfx/api/device.hpp"
#include "../../backends/vulkan/vk_device.hpp"

namespace ct::gfx {

result<ref<Device>> Device::Create(const DeviceInfo& info) {
    switch (info.backend) {
        case DeviceBackend::Vulkan:
            return vk::VKDeviceImpl::Create(info);
        case DeviceBackend::Metal:
            return err( ErrorCode::GRAPHICS_UNSUPPORTED_API, "Metal backend is not implemented yet");
        default:
            return err( ErrorCode::GRAPHICS_UNSUPPORTED_API, "Unsupported graphics backend");
    }
}

}
