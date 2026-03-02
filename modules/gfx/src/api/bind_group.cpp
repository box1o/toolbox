#include "../../include/toolbox/gfx/api/bind_group.hpp"
#include "toolbox/base/errors/result.hpp"

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/bind_group_impl.hpp"
#endif

namespace ct::gfx {

result<ref<BindGroup>> BindGroup::Builder::Build() noexcept {
#if defined(USE_WEBGPU_BACKEND)
    BindGroupDesc desc{};
    desc.layout = mLayout;
    desc.buffers = std::move(mBuffers);
    desc.textures = std::move(mTextures);
    desc.samplers = std::move(mSamplers);
    desc.debugName = std::move(mDebugName);

    auto bindGroup = createRef<webgpu::BindGroupImpl>(mDevice, std::move(desc));
    TRY_RETURN(bindGroup->Initialize());
    return ok(std::move(bindGroup));
#else
    log::Critical("BindGroup creation failed: no graphics backend available");
    std::abort();
#endif
}

// clang-format off
BindGroup::Builder& BindGroup::Builder::AddBuffer(u32 binding, ref<Buffer> buffer, u64 offset, u64 size) {
    mBuffers.push_back({binding, std::move(buffer), offset, size});
    return *this;
}

BindGroup::Builder& BindGroup::Builder::AddTexture(u32 binding, ref<TextureView> view) {
    mTextures.push_back({binding, std::move(view)});
    return *this;
}

BindGroup::Builder& BindGroup::Builder::AddSampler(u32 binding, ref<Sampler> sampler) {
    mSamplers.push_back({binding, std::move(sampler)});
    return *this;
}

BindGroup::Builder& BindGroup::Builder::SetDebugName(const std::string& name) {
    mDebugName = name;
    return *this;
}
// clang-format on

} // namespace ct::gfx
