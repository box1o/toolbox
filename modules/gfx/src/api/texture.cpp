#include <toolbox/gfx/api/texture_resource.hpp>
#include <toolbox/base/logger/logger.hpp>

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/texture_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Texture>> Texture::Create(ref<Device> device, const TextureDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto impl = createRef<webgpu::TextureImpl>();
    if (auto res = impl->Initialize(device, desc); !res) {
        return err(res.error());
    }
    return impl;
#else
    (void)device; (void)desc;
    log::Critical("Texture creation failed: no graphics backend");
    std::abort();
#endif
}

result<ref<Texture>> Texture::FromFile(ref<Device> device, const std::string& path, const TextureFromFileDesc& desc) noexcept {
#if defined(USE_WEBGPU_BACKEND)
    return webgpu::TextureImpl::FromFile(device, path, desc);
#else
    (void)device; (void)path; (void)desc;
    log::Critical("Texture::FromFile failed: no graphics backend");
    std::abort();
#endif
}

} // namespace ct::gfx
