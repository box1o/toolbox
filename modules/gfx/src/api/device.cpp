#include "../../include/toolbox/gfx/api/device.hpp"

#include "toolbox/base/errors/result.hpp"

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/device_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Device>> Device::Create(const DeviceDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto device = createRef<webgpu::DeviceImpl>(desc);
    TRY_RETURN(device->Initialize());
    // log out the adapter info
    auto& adapterInfo = device->GetAdapterInfo();
    log::Info("Device:");
    log::Info("\tVendor: {}", adapterInfo.vendor);
    log::Info("\tArchitecture: {}", adapterInfo.architecture);
    log::Info("\tGPU: {}", adapterInfo.device);
    log::Info("\tDriver: {}", adapterInfo.description);

    return ok(std::move(device));
#else
    (void)desc;
    log::Critical("Device creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
