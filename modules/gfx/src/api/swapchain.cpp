#include "../../include/toolbox/gfx/api/swapchain.hpp"

#include "toolbox/base/errors/result.hpp"

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/swapchain_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Swapchain>> Swapchain::Create(
    ref<Device> device, ref<Window> window, const SwapchainDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto swapchain = createRef<webgpu::SwapchainImpl>(device, window, desc);
    TRY_RETURN(swapchain->Initialize());
    return ok(std::move(swapchain));
#else
    (void)desc;
    log::Critical("Swapchain creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
