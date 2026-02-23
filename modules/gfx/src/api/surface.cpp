#include <toolbox/gfx/api/surface.hpp>

#include <toolbox/gfx/api/device.hpp>
#include <toolbox/base/logger/logger.hpp>

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/surface_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Surface>> Surface::Create(
    ref<Device> device, ref<Window> window, const SurfaceDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::SurfaceImpl>();

    if (auto res = impl->Initialize(device, window, desc); !res) {
        return err(res.error());
    }
    return impl;
#else
    (void)device;
    (void)window;
    (void)desc;
    log::Critical("Surface creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
