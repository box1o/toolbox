#include "bindings_impl.hpp"
#include "device_impl.hpp"
#include "buffer_impl.hpp"
#include "texture_impl.hpp"
#include "sampler_impl.hpp"

#include <toolbox/base/logger/logger.hpp>

namespace ct::gfx::webgpu {

static wgpu::ShaderStage ToWGPU(ShaderStageFlags s) noexcept {
    wgpu::ShaderStage out = wgpu::ShaderStage::None;
    if (HasFlag(s, ShaderStageFlags::Vertex))   out |= wgpu::ShaderStage::Vertex;
    if (HasFlag(s, ShaderStageFlags::Fragment)) out |= wgpu::ShaderStage::Fragment;
    if (HasFlag(s, ShaderStageFlags::Compute))  out |= wgpu::ShaderStage::Compute;
    return out;
}

bool BindGroupLayoutImpl::Init(ref<Device> device, const BindGroupLayoutDesc& desc) noexcept {
    if (!device) return false;

    auto* dev = dynamic_cast<DeviceImpl*>(device.get());
    if (!dev) {
        log::Error("BindGroupLayoutImpl: device backend mismatch");
        return false;
    }

    mDevice = dev->DeviceHandle();
    if (!mDevice) return false;

    std::vector<wgpu::BindGroupLayoutEntry> entries;
    entries.reserve(desc.bindings.size());

    for (const auto& b : desc.bindings) {
        wgpu::BindGroupLayoutEntry e{};
        e.binding = b.binding;
        e.visibility = ToWGPU(b.visibility);

        switch (b.type) {
            case BindingType::UniformBuffer:
                e.buffer.type = wgpu::BufferBindingType::Uniform;
                e.buffer.minBindingSize = b.minBindingSize;
                break;

            case BindingType::StorageBuffer:
                e.buffer.type = wgpu::BufferBindingType::Storage;
                e.buffer.minBindingSize = b.minBindingSize;
                break;

            case BindingType::Texture2D:
                e.texture.sampleType = wgpu::TextureSampleType::Float;
                e.texture.viewDimension = wgpu::TextureViewDimension::e2D;
                e.texture.multisampled = false;
                break;

            case BindingType::Sampler:
                e.sampler.type = wgpu::SamplerBindingType::Filtering;
                break;
        }

        entries.push_back(e);
    }

    wgpu::BindGroupLayoutDescriptor bgl{};
    bgl.entryCount = (uint32_t)entries.size();
    bgl.entries = entries.data();
    if (!desc.debugName.empty()) bgl.label = desc.debugName.c_str();

    mLayout = mDevice.CreateBindGroupLayout(&bgl);
    return mLayout != nullptr;
}

bool BindGroupImpl::Init(ref<Device> device, const BindGroupDesc& desc) noexcept {
    if (!device || !desc.layout) return false;

    auto* dev = dynamic_cast<DeviceImpl*>(device.get());
    if (!dev) {
        log::Error("BindGroupImpl: device backend mismatch");
        return false;
    }
    mDevice = dev->DeviceHandle();
    if (!mDevice) return false;

    auto* layoutImpl = dynamic_cast<BindGroupLayoutImpl*>(desc.layout.get());
    if (!layoutImpl) {
        log::Error("BindGroupImpl: layout backend mismatch");
        return false;
    }

    std::vector<wgpu::BindGroupEntry> entries;
    entries.reserve(desc.entries.size());

    // keep any wrapper refs alive during CreateBindGroup
    std::vector<wgpu::Buffer> keepBuffers;
    std::vector<wgpu::TextureView> keepViews;
    std::vector<wgpu::Sampler> keepSamplers;

    for (const auto& in : desc.entries) {
        wgpu::BindGroupEntry e{};
        e.binding = in.binding;

        switch (in.type) {
            case BindingType::UniformBuffer:
            case BindingType::StorageBuffer: {
                if (!in.buffer.buffer) return false;
                auto* b = dynamic_cast<BufferImpl*>(in.buffer.buffer.get());
                if (!b) return false;

                auto buf = b->Handle();
                keepBuffers.push_back(buf);

                e.buffer = buf;
                e.offset = in.buffer.offset;
                e.size   = in.buffer.size;
            } break;

            case BindingType::Texture2D: {
                if (!in.texture.view) return false;
                auto* tv = dynamic_cast<TextureViewImpl*>(in.texture.view.get());
                if (!tv) return false;

                auto view = tv->Handle();
                keepViews.push_back(view);

                e.textureView = view;
            } break;

            case BindingType::Sampler: {
                if (!in.sampler.sampler) return false;
                auto* s = dynamic_cast<SamplerImpl*>(in.sampler.sampler.get());
                if (!s) return false;

                auto sam = s->Handle();
                keepSamplers.push_back(sam);

                e.sampler = sam;
            } break;
        }

        entries.push_back(e);
    }

    wgpu::BindGroupDescriptor bg{};
    bg.layout = layoutImpl->Handle();
    bg.entryCount = (uint32_t)entries.size();
    bg.entries = entries.data();
    if (!desc.debugName.empty()) bg.label = desc.debugName.c_str();

    mGroup = mDevice.CreateBindGroup(&bg);
    return mGroup != nullptr;
}

bool PipelineLayoutImpl::Init(ref<Device> device, const PipelineLayoutDesc& desc) noexcept {
    if (!device) return false;

    auto* dev = dynamic_cast<DeviceImpl*>(device.get());
    if (!dev) {
        log::Error("PipelineLayoutImpl: device backend mismatch");
        return false;
    }

    mDevice = dev->DeviceHandle();
    if (!mDevice) return false;

    std::vector<wgpu::BindGroupLayout> layouts;
    layouts.reserve(desc.setLayouts.size());

    for (auto& l : desc.setLayouts) {
        auto* impl = dynamic_cast<BindGroupLayoutImpl*>(l.get());
        if (!impl) return false;
        layouts.push_back(impl->Handle());
    }

    wgpu::PipelineLayoutDescriptor pld{};
    pld.bindGroupLayoutCount = (uint32_t)layouts.size();
    pld.bindGroupLayouts = layouts.data();
    if (!desc.debugName.empty()) pld.label = desc.debugName.c_str();

    mLayout = mDevice.CreatePipelineLayout(&pld);
    return mLayout != nullptr;
}

} // namespace ct::gfx::webgpu
