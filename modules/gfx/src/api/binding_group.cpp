#include "toolbox/gfx/api/bind_group.hpp"
#include "toolbox/gfx/api/device.hpp"
#include "toolbox/gfx/api/buffer.hpp"
#include "toolbox/gfx/api/texture.hpp"
#include "toolbox/gfx/api/sampler.hpp"

namespace ct {

namespace {

wgpu::BufferBindingType ToWGPUBufferType(BindingType type) {
    switch (type) {
    case BindingType::UniformBuffer:          return wgpu::BufferBindingType::Uniform;
    case BindingType::StorageBuffer:          return wgpu::BufferBindingType::Storage;
    case BindingType::ReadOnlyStorageBuffer:  return wgpu::BufferBindingType::ReadOnlyStorage;
    default:                                  return wgpu::BufferBindingType::Undefined;
    }
}

bool IsBufferType(BindingType type) {
    return type == BindingType::UniformBuffer ||
           type == BindingType::StorageBuffer ||
           type == BindingType::ReadOnlyStorageBuffer;
}

bool IsTextureType(BindingType type) {
    return type == BindingType::Texture ||
           type == BindingType::StorageTexture;
}

bool IsSamplerType(BindingType type) {
    return type == BindingType::Sampler ||
           type == BindingType::ComparisonSampler;
}

} // namespace

// --- BindGroupLayout ---

result<ref<BindGroupLayout>> BindGroupLayout::Create(
    ref<Device> device, const BindGroupLayoutInfo& info) noexcept {

    if (!device) return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Device is null");

    std::vector<wgpu::BindGroupLayoutEntry> wgpuEntries;
    wgpuEntries.reserve(info.entries.size());

    for (const auto& entry : info.entries) {
        wgpu::BindGroupLayoutEntry wgpuEntry{};
        wgpuEntry.binding = entry.binding;
        wgpuEntry.visibility = entry.visibility;

        if (IsBufferType(entry.type)) {
            wgpuEntry.buffer.type = ToWGPUBufferType(entry.type);
            wgpuEntry.buffer.minBindingSize = entry.minBufferSize;
        } else if (entry.type == BindingType::Texture) {
            wgpuEntry.texture.sampleType = wgpu::TextureSampleType::Float;
            wgpuEntry.texture.viewDimension = wgpu::TextureViewDimension::e2D;
        } else if (entry.type == BindingType::StorageTexture) {
            wgpuEntry.storageTexture.access = wgpu::StorageTextureAccess::WriteOnly;
            wgpuEntry.storageTexture.format = wgpu::TextureFormat::RGBA8Unorm;
            wgpuEntry.storageTexture.viewDimension = wgpu::TextureViewDimension::e2D;
        } else if (entry.type == BindingType::Sampler) {
            wgpuEntry.sampler.type = wgpu::SamplerBindingType::Filtering;
        } else if (entry.type == BindingType::ComparisonSampler) {
            wgpuEntry.sampler.type = wgpu::SamplerBindingType::Comparison;
        }

        wgpuEntries.push_back(wgpuEntry);
    }

    wgpu::BindGroupLayoutDescriptor desc{};
    desc.entryCount = static_cast<u32>(wgpuEntries.size());
    desc.entries = wgpuEntries.data();

    ref<BindGroupLayout> layout(new BindGroupLayout());
    layout->mLayout = device->GetDevice().CreateBindGroupLayout(&desc);
    layout->mInfo = info;

    if (!layout->mLayout) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to create bind group layout");
    }

    log::Info("[wgpu] BindGroupLayout created ({} entries)", info.entries.size());
    return layout;
}

BindGroupLayout::~BindGroupLayout() {
    mLayout = nullptr;
}

wgpu::BindGroupLayout BindGroupLayout::GetHandle() const noexcept { return mLayout; }
const BindGroupLayoutInfo& BindGroupLayout::GetInfo() const noexcept { return mInfo; }

// --- BindGroup ---

result<ref<BindGroup>> BindGroup::Create(
    ref<Device> device, const BindGroupInfo& info) noexcept {

    if (!device) return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Device is null");
    if (!info.layout) return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "BindGroupLayout is null");

    std::vector<wgpu::BindGroupEntry> wgpuEntries;
    wgpuEntries.reserve(info.entries.size());

    for (const auto& entry : info.entries) {
        wgpu::BindGroupEntry wgpuEntry{};
        wgpuEntry.binding = entry.binding;

        if (auto* bufBinding = std::get_if<BufferBinding>(&entry.resource)) {
            if (!bufBinding->buffer) {
                return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Buffer is null at binding " + std::to_string(entry.binding));
            }
            wgpuEntry.buffer = bufBinding->buffer->GetHandle();
            wgpuEntry.offset = bufBinding->offset;
            wgpuEntry.size = (bufBinding->size > 0) ? bufBinding->size : bufBinding->buffer->GetSize();
        } else if (auto* tex = std::get_if<ref<Texture>>(&entry.resource)) {
            if (!*tex) {
                return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Texture is null at binding " + std::to_string(entry.binding));
            }
            wgpuEntry.textureView = (*tex)->GetView();
        } else if (auto* samp = std::get_if<ref<Sampler>>(&entry.resource)) {
            if (!*samp) {
                return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Sampler is null at binding " + std::to_string(entry.binding));
            }
            wgpuEntry.sampler = (*samp)->GetHandle();
        }

        wgpuEntries.push_back(wgpuEntry);
    }

    wgpu::BindGroupDescriptor desc{};
    desc.layout = info.layout->GetHandle();
    desc.entryCount = static_cast<u32>(wgpuEntries.size());
    desc.entries = wgpuEntries.data();

    ref<BindGroup> bindGroup(new BindGroup());
    bindGroup->mBindGroup = device->GetDevice().CreateBindGroup(&desc);

    if (!bindGroup->mBindGroup) {
        return err(ErrorCode::FAILED_TO_ACQUIRE_RESOURCE, "Failed to create bind group");
    }

    log::Info("[wgpu] BindGroup created ({} entries)", info.entries.size());
    return bindGroup;
}

BindGroup::~BindGroup() {
    mBindGroup = nullptr;
}

wgpu::BindGroup BindGroup::GetHandle() const noexcept { return mBindGroup; }

} // namespace ct
