#include "ct/gfx/api/device.hpp"

#include "../../backends/vulkan/vk_device.hpp"
#include <ct/base/base.hpp>

namespace ct::gfx {

result<ref<Device>> Device::Create(const DeviceInfo& info) {
    switch (info.backend) {
    case DeviceBackend::Vulkan:
        return createRef<vk::VKDeviceImpl>(info);

    case DeviceBackend::Metal:
        log::Critical("Metal backend is not supported on this platform");
        return err(
            ErrorCode::GRAPHICS_UNSUPPORTED_API,
            "Metal backend is not implemented yet"
        );

    default:
        log::Critical("Unsupported device backend specified");
        return err(
            ErrorCode::GRAPHICS_UNSUPPORTED_API,
            "This platform does not support the specified graphics API backend"
        );
    }
}

} // namespace ct::gfx
