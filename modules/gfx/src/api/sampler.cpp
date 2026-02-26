#include "../../include/toolbox/gfx/api/sampler.hpp"

#include "toolbox/base/errors/result.hpp"

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/sampler_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Sampler>> Sampler::Create(ref<Device> device, const SamplerDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto queue = createRef<webgpu::SamplerImpl>(device, desc);
    TRY_RETURN(queue->Initialize());
    return ok(std::move(queue));
#else
    (void)desc;
    log::Critical("Sampler creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
