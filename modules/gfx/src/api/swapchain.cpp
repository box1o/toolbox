#include <toolbox/gfx/api/swapchain.hpp>
#include <toolbox/gfx/api/device.hpp>
#include <toolbox/gfx/api/surface.hpp>
#include <toolbox/base/logger/logger.hpp>
#include <cstdlib>

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/swapchain_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Swapchain>> Swapchain::Create(ref<Device> device, ref<Surface> surface, const SwapchainDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::SwapchainImpl>();
    if (!impl->Init(device, surface, desc)) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Swapchain: init failed");
    }
    log::Info("[gfx] Swapchain created (WebGPU)");
    return impl;
#else
    (void)device; (void)surface; (void)desc;
    log::Critical("Swapchain creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
