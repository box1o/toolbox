#include <toolbox/gfx/api/pipeline.hpp>
#include <toolbox/base/logger/logger.hpp>
#include <cstdlib>

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/pipeline_impl.hpp"
#endif

namespace ct::gfx {

result<ref<RenderPipeline>> RenderPipeline::Create(ref<Device> device, const RenderPipelineDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::RenderPipelineImpl>();
    if (!impl->Init(device, desc)) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "RenderPipeline: init failed");
    }
    return impl;
#else
    (void)device; (void)desc;
    log::Critical("RenderPipeline creation failed: no graphics backend available");
    std::abort();
#endif
}

} // namespace ct::gfx
