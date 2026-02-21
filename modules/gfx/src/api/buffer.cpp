#include <toolbox/gfx/api/buffer.hpp>
#include <toolbox/base/logger/logger.hpp>
#include <cstdlib>

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/buffer_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Buffer>> Buffer::Create(ref<Device> device, const BufferDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::BufferImpl>();
    if (!impl->Init(device, desc)) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Buffer: init failed");
    }
    return impl;
#else
    (void)device; (void)desc;
    log::Critical("Buffer creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
