#include "ct/gfx/api/device.hpp"
#include "../../backends/vulkan/vk_device.hpp"
#include "ct/base/types/types.hpp"

namespace ct::gfx {

ref<Device> Device::Create(const DeviceInfo& info) {
    switch (info.backend) {
        case DeviceBackend::Vulkan:
            return createRef<vk::VKDeviceImpl>(info);
        case DeviceBackend::Metal:
            log::Critical("Metal backend is not implemented yet");
            std::abort();
        default:
            log::Critical("Unsupported graphics backend");
            std::abort();
    }
}

}
