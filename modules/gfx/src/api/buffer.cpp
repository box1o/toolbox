#include <toolbox/gfx/api/buffer.hpp>
#include <toolbox/base/logger/logger.hpp>

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/buffer_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Buffer>> Buffer::Create(ref<Device> device, const BufferDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::BufferImpl>();
    if (auto res = impl->Initialize(device, desc) ; !res) {
        return err(res.error());
    }
    return impl;
#else
    (void)device; (void)desc;
    log::Critical("Buffer creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
