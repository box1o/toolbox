#include "../../include/toolbox/gfx/api/device.hpp"
#include "toolbox/base/logger/logger.hpp"
#include <cstdlib>

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/device.hpp"
#endif

namespace ct::gfx {

result<ref<Device>> Device::Create(const DeviceDesc& desc, bool async) noexcept {
    (void)async; // async is not implemented in this backend

#if defined(USE_WEBGPU_BACKEND)
    auto deviceImpl = createRef<WGPUDeviceImpl>(desc);

    if (!deviceImpl->CreateInstance()) { return err(ErrorCode::GRAPHICS_INIT_FAILED, "Device: failed to create instance"); }
    if (!deviceImpl->RequestAdapter(desc)) { return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Device: failed to acquire adapter"); }
    deviceImpl->QueryAdapterInfo();

    if (!deviceImpl->RequestDevice(desc)) { return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Device: failed to acquire device"); }
    deviceImpl->QueryLimits();

    log::Info("[wgpu] Device created");
    return deviceImpl;
#endif

    log::Critical("Device creation failed: no graphics backend available");
    std::abort();
}

} // namespace ct::gfx
