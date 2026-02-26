#pragma once
#include <toolbox/gfx/api/buffer.hpp>
#include <toolbox/gfx/api/device.hpp>
#include <toolbox/gfx/api/texture.hpp>
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx::detail {

constexpr wgpu::PowerPreference ToWGPU(PowerProfile p) noexcept {
    switch (p) {
    case PowerProfile::LowPower:
        return wgpu::PowerPreference::LowPower;
    case PowerProfile::HighPerformance:
        return wgpu::PowerPreference::HighPerformance;
    default:
        return wgpu::PowerPreference::Undefined;
    }
}

constexpr std::string ToString(wgpu::StringView sv) noexcept {
    if (!sv.data || sv.length == 0) return {};
    return std::string(sv.data, sv.length);
}


// NOTE: Buffer
constexpr wgpu::BufferUsage ToWGPU(BufferUsageFlags usage) noexcept {
    wgpu::BufferUsage out = wgpu::BufferUsage::None;

    if (HasFlag(usage, BufferUsageFlags::Vertex)) out |= wgpu::BufferUsage::Vertex;
    if (HasFlag(usage, BufferUsageFlags::Index)) out |= wgpu::BufferUsage::Index;
    if (HasFlag(usage, BufferUsageFlags::Uniform)) out |= wgpu::BufferUsage::Uniform;
    if (HasFlag(usage, BufferUsageFlags::Storage)) out |= wgpu::BufferUsage::Storage;
    if (HasFlag(usage, BufferUsageFlags::CopySrc)) out |= wgpu::BufferUsage::CopySrc;
    if (HasFlag(usage, BufferUsageFlags::CopyDst)) out |= wgpu::BufferUsage::CopyDst;
    if (HasFlag(usage, BufferUsageFlags::MapRead)) out |= wgpu::BufferUsage::MapRead;
    if (HasFlag(usage, BufferUsageFlags::MapWrite)) out |= wgpu::BufferUsage::MapWrite;
    if (HasFlag(usage, BufferUsageFlags::Indirect)) out |= wgpu::BufferUsage::Indirect;
    if (HasFlag(usage, BufferUsageFlags::QueryResolve)) out |= wgpu::BufferUsage::QueryResolve;

    return out;
}


// NOTE: Texture
constexpr wgpu::TextureFormat ToWGPU(TextureFormat fmt) noexcept {
    switch (fmt) {
    case TextureFormat::RGBA8Unorm: return wgpu::TextureFormat::RGBA8Unorm;
    case TextureFormat::RGBA8UnormSrgb: return wgpu::TextureFormat::RGBA8UnormSrgb;
    case TextureFormat::BGRA8Unorm: return wgpu::TextureFormat::BGRA8Unorm;
    case TextureFormat::BGRA8UnormSrgb: return wgpu::TextureFormat::BGRA8UnormSrgb;
    case TextureFormat::Depth16Unorm: return wgpu::TextureFormat::Depth16Unorm;
    case TextureFormat::Depth24Plus: return wgpu::TextureFormat::Depth24Plus;
    case TextureFormat::Depth24PlusStencil8: return wgpu::TextureFormat::Depth24PlusStencil8;
    case TextureFormat::Depth32Float: return wgpu::TextureFormat::Depth32Float;
    case TextureFormat::Depth32FloatStencil8: return wgpu::TextureFormat::Depth32FloatStencil8;
    default: return wgpu::TextureFormat::Undefined;
    }
}

constexpr wgpu::TextureUsage ToWGPUUsage(TextureUsageFlags usage) noexcept {
    wgpu::TextureUsage out = wgpu::TextureUsage::None;
    if (HasFlag(usage, TextureUsageFlags::Sampled)) out |= wgpu::TextureUsage::TextureBinding;
    if (HasFlag(usage, TextureUsageFlags::Storage)) out |= wgpu::TextureUsage::StorageBinding;
    if (HasFlag(usage, TextureUsageFlags::RenderTarget))
        out |= wgpu::TextureUsage::RenderAttachment;
    if (HasFlag(usage, TextureUsageFlags::CopySrc)) out |= wgpu::TextureUsage::CopySrc;
    if (HasFlag(usage, TextureUsageFlags::CopyDst)) out |= wgpu::TextureUsage::CopyDst;
    return out;
}

} // namespace ct::gfx::detail
