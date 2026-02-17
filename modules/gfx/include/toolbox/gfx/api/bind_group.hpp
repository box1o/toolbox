#pragma once
#include "toolbox/base/base.hpp"
#include <webgpu/webgpu_cpp.h>
#include <vector>
#include <variant>

namespace ct {

class Device;
class Buffer;
class Texture;
class Sampler;

enum class BindingType : u8 {
    UniformBuffer,
    StorageBuffer,
    ReadOnlyStorageBuffer,
    Texture,
    StorageTexture,
    Sampler,
    ComparisonSampler,
};

struct BindGroupLayoutEntry {
    u32 binding{0};
    BindingType type{BindingType::UniformBuffer};
    wgpu::ShaderStage visibility{wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment};
    u64 minBufferSize{0};
};

struct BindGroupLayoutInfo {
    std::vector<BindGroupLayoutEntry> entries{};

    BindGroupLayoutInfo& AddUniform(u32 binding, u64 minSize,
        wgpu::ShaderStage visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment) {
        entries.push_back({binding, BindingType::UniformBuffer, visibility, minSize});
        return *this;
    }

    BindGroupLayoutInfo& AddStorage(u32 binding, u64 minSize, bool readOnly = false,
        wgpu::ShaderStage visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment) {
        entries.push_back({binding,
            readOnly ? BindingType::ReadOnlyStorageBuffer : BindingType::StorageBuffer,
            visibility, minSize});
        return *this;
    }

    BindGroupLayoutInfo& AddTexture(u32 binding,
        wgpu::ShaderStage visibility = wgpu::ShaderStage::Fragment) {
        entries.push_back({binding, BindingType::Texture, visibility, 0});
        return *this;
    }

    BindGroupLayoutInfo& AddStorageTexture(u32 binding,
        wgpu::ShaderStage visibility = wgpu::ShaderStage::Fragment) {
        entries.push_back({binding, BindingType::StorageTexture, visibility, 0});
        return *this;
    }

    BindGroupLayoutInfo& AddSampler(u32 binding, bool comparison = false,
        wgpu::ShaderStage visibility = wgpu::ShaderStage::Fragment) {
        entries.push_back({binding,
            comparison ? BindingType::ComparisonSampler : BindingType::Sampler,
            visibility, 0});
        return *this;
    }
};

class BindGroupLayout {
public:
    ~BindGroupLayout();

    [[nodiscard]] wgpu::BindGroupLayout GetHandle() const noexcept;
    [[nodiscard]] const BindGroupLayoutInfo& GetInfo() const noexcept;

    [[nodiscard]] static result<ref<BindGroupLayout>> Create(
        ref<Device> device, const BindGroupLayoutInfo& info) noexcept;

private:
    BindGroupLayout() = default;

    wgpu::BindGroupLayout mLayout{nullptr};
    BindGroupLayoutInfo mInfo{};
};

struct BufferBinding {
    ref<Buffer> buffer;
    u64 offset{0};
    u64 size{0};
};

struct BindGroupEntry {
    u32 binding{0};
    std::variant<BufferBinding, ref<Texture>, ref<Sampler>> resource;
};

struct BindGroupInfo {
    ref<BindGroupLayout> layout;
    std::vector<BindGroupEntry> entries{};

    BindGroupInfo& SetLayout(ref<BindGroupLayout> l) {
        layout = std::move(l);
        return *this;
    }

    //NOTE: size=0 means use the full buffer
    BindGroupInfo& AddBuffer(u32 binding, ref<Buffer> buffer, u64 offset = 0, u64 size = 0) {
        entries.push_back({binding, BufferBinding{std::move(buffer), offset, size}});
        return *this;
    }

    BindGroupInfo& AddTexture(u32 binding, ref<Texture> texture) {
        entries.push_back({binding, std::move(texture)});
        return *this;
    }

    BindGroupInfo& AddSampler(u32 binding, ref<Sampler> sampler) {
        entries.push_back({binding, std::move(sampler)});
        return *this;
    }
};

class BindGroup {
public:
    ~BindGroup();

    [[nodiscard]] wgpu::BindGroup GetHandle() const noexcept;

    [[nodiscard]] static result<ref<BindGroup>> Create(
        ref<Device> device, const BindGroupInfo& info) noexcept;

private:
    BindGroup() = default;

    wgpu::BindGroup mBindGroup{nullptr};
};

} // namespace ct
