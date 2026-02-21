#include <toolbox/gfx/api/surface.hpp>
#include <toolbox/gfx/api/device.hpp>
#include <toolbox/base/logger/logger.hpp>
#include <cstdlib>

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/surface_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Surface>> Surface::Create(ref<Device> device, ref<Window> window, const SurfaceDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::SurfaceImpl>();
    if (!impl->Init(device, window, desc)) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Surface: failed to create native surface");
    }
    log::Info("[gfx] Surface created (WebGPU)");
    return impl;
#else
    (void)device; (void)window; (void)desc;
    log::Critical("Surface creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
