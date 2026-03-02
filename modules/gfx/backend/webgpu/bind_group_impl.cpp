#include "bind_group_impl.hpp"
#include "common.hpp"
#include "toolbox/gfx/api/buffer.hpp"
#include "toolbox/gfx/api/device.hpp"
#include "toolbox/gfx/api/sampler.hpp"
#include "toolbox/gfx/api/texture.hpp"

namespace ct::gfx::webgpu {

BindGroupImpl::BindGroupImpl(ref<Device> device, BindGroupDesc desc)
    : mDesc(std::move(desc)), mDevice(std::move(device)) {}

result<wgpu::BindGroupLayout> BindGroupImpl::CreateNativeLayout(wgpu::Device& device) noexcept {
    const auto& layoutEntries = mDesc.layout.GetEntries();

    std::vector<wgpu::BindGroupLayoutEntry> nativeEntries;
    nativeEntries.reserve(layoutEntries.size());

    for (const auto& entry : layoutEntries) {
        wgpu::BindGroupLayoutEntry bgle{};
        bgle.binding = entry.binding;
        bgle.visibility = detail::ToWGPUVisibility(entry.visibility);

        switch (entry.type) {
        case BindingType::UniformBuffer:
        case BindingType::StorageBuffer:
        case BindingType::ReadOnlyStorageBuffer: {
            wgpu::BufferBindingLayout buf{};
            buf.type = detail::ToWGPUBufferBinding(entry.type);
            buf.minBindingSize = entry.bufferSize;
            buf.hasDynamicOffset = entry.bufferHasDynamicOffset;
            bgle.buffer = buf;
            break;
        }
        case BindingType::Sampler: {
            wgpu::SamplerBindingLayout sam{};
            sam.type = wgpu::SamplerBindingType::Filtering;
            bgle.sampler = sam;
            break;
        }
        case BindingType::Texture: {
            wgpu::TextureBindingLayout tex{};
            tex.sampleType = detail::ToWGPU(entry.textureSampleType);
            tex.viewDimension = detail::ToWGPU(entry.textureViewDimension);
            tex.multisampled = false;
            bgle.texture = tex;
            break;
        }
        case BindingType::StorageTexture: {
            wgpu::StorageTextureBindingLayout stex{};
            stex.access = wgpu::StorageTextureAccess::WriteOnly;
            stex.format = detail::ToWGPU(entry.storageTextureFormat);
            stex.viewDimension = detail::ToWGPU(entry.textureViewDimension);
            bgle.storageTexture = stex;
            break;
        }
        }

        nativeEntries.push_back(bgle);
    }

    wgpu::BindGroupLayoutDescriptor bgld{};
    bgld.entryCount = nativeEntries.size();
    bgld.entries = nativeEntries.data();

    auto nativeLayout = device.CreateBindGroupLayout(&bgld);
    if (!nativeLayout) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "BindGroup: failed to create bind group layout");
    }

    return ok(nativeLayout);
}

result<void> BindGroupImpl::Initialize() noexcept {
    if (!mDevice) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "BindGroup: device is null");
    }

    auto* nativeDevice = static_cast<wgpu::Device*>(mDevice->GetNativeDeviceHandle());
    if (!nativeDevice || !*nativeDevice) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "BindGroup: failed to acquire device handle");
    }

    // NOTE: Create the native bind group layout from our abstract layout
    auto layoutResult = CreateNativeLayout(*nativeDevice);
    if (!layoutResult) return err(layoutResult.error());
    mNativeLayout = layoutResult.value();

    // NOTE: Collect all bind group entries
    const u32 totalEntries =
        static_cast<u32>(mDesc.buffers.size() + mDesc.textures.size() + mDesc.samplers.size());

    std::vector<wgpu::BindGroupEntry> entries;
    entries.reserve(totalEntries);

    // NOTE: Buffer bindings
    for (const auto& buf : mDesc.buffers) {
        if (!buf.buffer) {
            return err(ErrorCode::INVALID_ARGUMENT,
                "BindGroup: buffer binding is null at slot " + std::to_string(buf.binding));
        }

        auto* nativeBuffer = static_cast<wgpu::Buffer*>(buf.buffer->GetNativeBufferHandle());
        if (!nativeBuffer || !*nativeBuffer) {
            return err(ErrorCode::INVALID_STATE,
                "BindGroup: buffer handle is null at slot " + std::to_string(buf.binding));
        }

        u64 effectiveSize = (buf.size == 0) ? (buf.buffer->GetSize() - buf.offset) : buf.size;

        wgpu::BindGroupEntry bge{};
        bge.binding = buf.binding;
        bge.buffer = *nativeBuffer;
        bge.offset = buf.offset;
        bge.size = effectiveSize;
        entries.push_back(bge);
    }

    // NOTE: Texture bindings
    for (const auto& tex : mDesc.textures) {
        if (!tex.view) {
            return err(ErrorCode::INVALID_ARGUMENT,
                "BindGroup: texture view is null at slot " + std::to_string(tex.binding));
        }

        auto* nativeView = static_cast<wgpu::TextureView*>(tex.view->GetNativeTextureViewHandle());
        if (!nativeView || !*nativeView) {
            return err(ErrorCode::INVALID_STATE,
                "BindGroup: texture view handle is null at slot " + std::to_string(tex.binding));
        }

        wgpu::BindGroupEntry bge{};
        bge.binding = tex.binding;
        bge.textureView = *nativeView;
        entries.push_back(bge);
    }

    // NOTE: Sampler bindings
    for (const auto& sam : mDesc.samplers) {
        if (!sam.sampler) {
            return err(ErrorCode::INVALID_ARGUMENT,
                "BindGroup: sampler is null at slot " + std::to_string(sam.binding));
        }

        auto* nativeSampler = static_cast<wgpu::Sampler*>(sam.sampler->GetNativeSamplerHandle());
        if (!nativeSampler || !*nativeSampler) {
            return err(ErrorCode::INVALID_STATE,
                "BindGroup: sampler handle is null at slot " + std::to_string(sam.binding));
        }

        wgpu::BindGroupEntry bge{};
        bge.binding = sam.binding;
        bge.sampler = *nativeSampler;
        entries.push_back(bge);
    }

    // NOTE: Create the bind group
    wgpu::BindGroupDescriptor bgd{};
    bgd.label = wgpu::StringView{mDesc.debugName.c_str(), mDesc.debugName.size()};
    bgd.layout = mNativeLayout;
    bgd.entryCount = entries.size();
    bgd.entries = entries.data();

    mBindGroup = nativeDevice->CreateBindGroup(&bgd);
    if (!mBindGroup) {
        return err(
            ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "BindGroup: CreateBindGroup failed");
    }

    return ok();
}

} // namespace ct::gfx::webgpu
