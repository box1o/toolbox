#include <toolbox/base/logger/logger.hpp>
#include <toolbox/gfx/api/device.hpp>

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/device_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Device>> Device::Create(const DeviceDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::DeviceImpl>(desc);
    if (auto res = impl->Initialize(); !res) {
        return err(res.error());
    }
    return ok(impl);

#else
    (void)desc;
    log::Critical("Device creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
