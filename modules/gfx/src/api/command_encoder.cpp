#include <toolbox/gfx/api/command_encoder.hpp>
#include <toolbox/base/logger/logger.hpp>

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/command_impl.hpp"
#endif

namespace ct::gfx {

result<ref<CommandEncoder>> CommandEncoder::Create(ref<Device> device, const CommandEncoderDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::CommandEncoderImpl>();
    if (!impl->Init(device, desc)) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "CommandEncoder: init failed");
    }
    return impl;
#else
    (void)device; (void)desc;
    log::Critical("CommandEncoder creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
