#include "../../include/toolbox/gfx/api/queue.hpp"

#include "toolbox/base/errors/result.hpp"

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/queue_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Queue>> Queue::Create(ref<Device> device, const QueueDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto queue = createRef<webgpu::QueueImpl>(device, desc);
    TRY_RETURN(queue->Initialize());
    return ok(std::move(queue));
#else
    (void)desc;
    log::Critical("Queue creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
