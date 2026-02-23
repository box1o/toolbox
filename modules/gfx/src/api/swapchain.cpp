#include <toolbox/base/logger/logger.hpp>
#include <toolbox/gfx/api/device.hpp>
#include <toolbox/gfx/api/surface.hpp>
#include <toolbox/gfx/api/swapchain.hpp>

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/swapchain_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Swapchain>> Swapchain::Create(
    ref<Device> device, ref<Surface> surface, const SwapchainDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::SwapchainImpl>();

    if (auto res = impl->Initialize(device, surface, desc); !res) {
        return err(res.error());
    }
    return impl;
#else
    (void)device;
    (void)surface;
    (void)desc;
    log::Critical("Swapchain creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
