#include <toolbox/gfx/api/bindings.hpp>
#include <toolbox/base/logger/logger.hpp>
#include <cstdlib>

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/bindings_impl.hpp"
#endif

namespace ct::gfx {

result<ref<BindGroupLayout>> BindGroupLayout::Create(ref<Device> device, const BindGroupLayoutDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::BindGroupLayoutImpl>();
    if (!impl->Init(device, desc)) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "BindGroupLayout: init failed");
    }
    return impl;
#else
    (void)device; (void)desc;
    log::Critical("BindGroupLayout creation failed: no graphics backend");
    std::abort();
#endif
}

result<ref<BindGroup>> BindGroup::Create(ref<Device> device, const BindGroupDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::BindGroupImpl>();
    if (!impl->Init(device, desc)) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "BindGroup: init failed");
    }
    return impl;
#else
    (void)device; (void)desc;
    log::Critical("BindGroup creation failed: no graphics backend");
    std::abort();
#endif
}

result<ref<PipelineLayout>> PipelineLayout::Create(ref<Device> device, const PipelineLayoutDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::PipelineLayoutImpl>();
    if (!impl->Init(device, desc)) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "PipelineLayout: init failed");
    }
    return impl;
#else
    (void)device; (void)desc;
    log::Critical("PipelineLayout creation failed: no graphics backend");
    std::abort();
#endif
}

} // namespace ct::gfx
