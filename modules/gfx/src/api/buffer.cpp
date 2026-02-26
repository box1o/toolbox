#include "../../include/toolbox/gfx/api/buffer.hpp"

#include "toolbox/base/errors/result.hpp"

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/buffer_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Buffer>> Buffer::Create(ref<Device> device, const BufferDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto buffer = createRef<webgpu::BufferImpl>(device, desc);
    TRY_RETURN(buffer->Initialize());
    return ok(std::move(buffer));
#else
    (void)desc;
    log::Critical("Buffer creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
