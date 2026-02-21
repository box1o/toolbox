#include <toolbox/gfx/api/shader.hpp>
#include <toolbox/base/logger/logger.hpp>

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/shader_impl.hpp"
#endif

namespace ct::gfx {

result<ref<ShaderModule>> ShaderModule::FromWGSL(ref<Device> device, const std::string& wgsl, const ShaderDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::ShaderModuleImpl>();
    if (!impl->InitFromWGSL(device, wgsl, desc)) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "ShaderModule: init WGSL failed");
    }
    return impl;
#else
    (void)device; (void)wgsl; (void)desc;
    log::Critical("ShaderModule creation failed: no graphics backend");
    std::abort();
#endif
}

result<ref<ShaderModule>> ShaderModule::FromFile(ref<Device> device, const std::string& path, const ShaderFromFileDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::ShaderModuleImpl>();
    if (!impl->InitFromFile(device, path, desc)) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "ShaderModule: init file failed");
    }
    return impl;
#else
    (void)device; (void)path; (void)desc;
    log::Critical("ShaderModule creation failed: no graphics backend");
    std::abort();
#endif
}

} // namespace ct::gfx
