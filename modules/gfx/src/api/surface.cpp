#include "../../include/toolbox/gfx/api/surface.hpp"
#include "../../include/toolbox/gfx/api/device.hpp"
#include "toolbox/base/logger/logger.hpp"

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/surface.hpp"
#endif

namespace ct::gfx {

result<ref<Surface>> Surface::Create(ref<Device> device, ref<Window> window) noexcept {
#if defined(USE_WEBGPU_BACKEND)

    auto* inst = static_cast<wgpu::Instance*>(device->GetInstance());
    if (!inst || !(*inst)) {
        return err(ErrorCode::GRAPHICS_INIT_FAILED, "Surface: invalid WebGPU instance");
    }
    auto surfaceImpl = createRef<WGPUSurfaceImpl>();
    if (!surfaceImpl->CreateSurface(*inst, window)) {
        return err(ErrorCode::GRAPHICS_INIT_FAILED, "Surface: failed to create surface");
    }

    log::Info("[wgpu] Surface created");
    return surfaceImpl;
#endif

    log::Critical("Surface creation failed: no graphics backend available");
    return {};
}

} // namespace ct::gfx
