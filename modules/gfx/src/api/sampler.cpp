#include <toolbox/gfx/api/sampler.hpp>
#include <toolbox/base/logger/logger.hpp>
#include <cstdlib>

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/sampler_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Sampler>> Sampler::Create(ref<Device> device, const SamplerDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::SamplerImpl>();
    if (!impl->Init(device, desc)) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Sampler: init failed");
    }
    return impl;
#else
    (void)device; (void)desc;
    log::Critical("Sampler creation failed: no graphics backend");
    std::abort();
#endif
}

} // namespace ct::gfx
