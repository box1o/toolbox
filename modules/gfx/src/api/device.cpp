#include <toolbox/gfx/api/device.hpp>
#include <toolbox/base/logger/logger.hpp>

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/device_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Device>> Device::Create(const DeviceDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::DeviceImpl>(desc);

    if (!impl->CreateInstance()) {
        return err(ErrorCode::GRAPHICS_INIT_FAILED, "Device: failed to create instance");
    }
    if (!impl->RequestAdapter()) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Device: failed to request adapter");
    }
    impl->QueryAdapterInfo();

    if (!impl->RequestDevice()) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Device: failed to request device");
    }
    impl->QueryLimits();

    log::Info("[gfx] Device created (WebGPU)");
    return impl;
#else
    (void)desc;
    log::Critical("Device creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
