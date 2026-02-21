#include <toolbox/gfx/api/queue.hpp>
#include <toolbox/base/logger/logger.hpp>
#include <cstdlib>

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/queue_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Queue>> Queue::Create(ref<Device> device, const QueueDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::QueueImpl>();
    if (!impl->Init(device, desc)) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Queue: init failed");
    }
    return impl;
#else
    (void)device; (void)desc;
    log::Critical("Queue creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
