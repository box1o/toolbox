#include <toolbox/gfx/api/queue.hpp>

#include <toolbox/base/logger/logger.hpp>

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/queue_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Queue>> Queue::Create(ref<Device> device, const QueueDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::QueueImpl>();
    if (auto res = impl->Initialize(device, desc); !res) {
        return err(res.error());
    }
    return impl;
#else
    (void)device;
    (void)desc;
    log::Critical("Queue creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
