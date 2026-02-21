#include "../../include/toolbox/gfx/api/swapchain.hpp"
#include "toolbox/base/logger/logger.hpp"

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/swapchain.hpp"

#endif

namespace ct::gfx {

result<ref<Swapchain>> Swapchain::Create(ref<Device> device, ref<Window> window) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto swapchainImpl = createRef<WGPUSwapchainImpl>(device , window);
    log::Info("[wgpu] Swapchain created");
    return swapchainImpl;
#endif



}

} // namespace ct::gfx
