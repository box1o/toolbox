#include "toolbox/gfx/api/sampler.hpp"

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/sampler_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Sampler>> Sampler::Create(ref<Device> device, const SamplerDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto sampler = createRef<webgpu::SamplerImpl>(device, desc);
    TRY_RETURN(sampler->Initialize());
    return ok(std::move(sampler));
#else
    log::Critical("Sampler creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
