#pragma once
#include <toolbox/base/base.hpp>
#include <toolbox/gfx/api/types.hpp>
#include <vector>

namespace ct::gfx {

class Device;
class Buffer;
class TextureView;
class Sampler;

enum class ShaderStageFlags : u32 {
    None     = 0,
    Vertex   = 1u << 0,
    Fragment = 1u << 1,
    Compute  = 1u << 2,
};

[[nodiscard]] constexpr ShaderStageFlags operator|(ShaderStageFlags a, ShaderStageFlags b) noexcept {
    return static_cast<ShaderStageFlags>(static_cast<u32>(a) | static_cast<u32>(b));
}
[[nodiscard]] constexpr bool HasFlag(ShaderStageFlags v, ShaderStageFlags f) noexcept {
    return (static_cast<u32>(v) & static_cast<u32>(f)) != 0u;
}

enum class BindingType : u8 {
    UniformBuffer,
    StorageBuffer,
    Texture2D,
    Sampler,
};

struct BindingLayoutDesc {
    u32 binding{0};
    BindingType type{BindingType::UniformBuffer};
    ShaderStageFlags visibility{ShaderStageFlags::Vertex};

    // buffer range
    u64 minBindingSize{0};

    std::string debugName{};
};

struct BindGroupLayoutDesc {
    std::vector<BindingLayoutDesc> bindings{};
    std::string debugName{"BindGroupLayout"};

    BindGroupLayoutDesc& Add(const BindingLayoutDesc& b) {
        bindings.push_back(b);
        return *this;
    }
};

class BindGroupLayout {
public:
    virtual ~BindGroupLayout() = default;
    [[nodiscard]] virtual void* GetNativeLayout() const noexcept = 0;

    [[nodiscard]] static result<ref<BindGroupLayout>> Create(
        ref<Device> device, const BindGroupLayoutDesc& desc) noexcept;

protected:
    BindGroupLayout() = default;
};

struct BufferBinding {
    ref<Buffer> buffer{};
    u64 offset{0};
    u64 size{0}; // required for uniform buffers
};

struct TextureBinding {
    ref<TextureView> view{};
};

struct SamplerBinding {
    ref<Sampler> sampler{};
};

struct BindGroupEntry {
    u32 binding{0};
    BindingType type{BindingType::UniformBuffer};

    BufferBinding buffer{};
    TextureBinding texture{};
    SamplerBinding sampler{};
};

struct BindGroupDesc {
    ref<BindGroupLayout> layout{};
    std::vector<BindGroupEntry> entries{};
    std::string debugName{"BindGroup"};

    BindGroupDesc& BindUniformBuffer(u32 binding, ref<Buffer> buf, u64 offset, u64 size) {
        BindGroupEntry e{};
        e.binding = binding;
        e.type = BindingType::UniformBuffer;
        e.buffer = {buf, offset, size};
        entries.push_back(std::move(e));
        return *this;
    }

    BindGroupDesc& BindTexture2D(u32 binding, ref<TextureView> view) {
        BindGroupEntry e{};
        e.binding = binding;
        e.type = BindingType::Texture2D;
        e.texture = {view};
        entries.push_back(std::move(e));
        return *this;
    }

    BindGroupDesc& BindSampler(u32 binding, ref<Sampler> s) {
        BindGroupEntry e{};
        e.binding = binding;
        e.type = BindingType::Sampler;
        e.sampler = {s};
        entries.push_back(std::move(e));
        return *this;
    }
};

class BindGroup {
public:
    virtual ~BindGroup() = default;
    [[nodiscard]] virtual void* GetNativeGroup() const noexcept = 0;

    [[nodiscard]] static result<ref<BindGroup>> Create(
        ref<Device> device, const BindGroupDesc& desc) noexcept;

protected:
    BindGroup() = default;
};

struct PipelineLayoutDesc {
    std::vector<ref<BindGroupLayout>> setLayouts{};
    std::string debugName{"PipelineLayout"};

    PipelineLayoutDesc& Add(ref<BindGroupLayout> l) {
        setLayouts.push_back(l);
        return *this;
    }
};

class PipelineLayout {
public:
    virtual ~PipelineLayout() = default;
    [[nodiscard]] virtual void* GetNativeLayout() const noexcept = 0;

    [[nodiscard]] static result<ref<PipelineLayout>> Create(
        ref<Device> device, const PipelineLayoutDesc& desc) noexcept;

protected:
    PipelineLayout() = default;
};

} // namespace ct::gfx
