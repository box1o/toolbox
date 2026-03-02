#include "../../include/toolbox/gfx/api/command_encoder.hpp"
#include "toolbox/base/errors/result.hpp"

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/command_encoder_impl.hpp"
#endif

namespace ct::gfx {

result<ref<CommandEncoder>> CommandEncoder::Create(
    ref<Device> device, const CommandEncoderDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto encoder = createRef<webgpu::CommandEncoderImpl>(std::move(device), desc);
    TRY_RETURN(encoder->Initialize());
    return ok(std::move(encoder));
#else
    (void)device;
    (void)desc;
    log::Critical("CommandEncoder creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
