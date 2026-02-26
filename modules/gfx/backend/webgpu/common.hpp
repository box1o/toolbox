#pragma once
#include <toolbox/gfx/api/buffer.hpp>
#include <toolbox/gfx/api/device.hpp>
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

} // namespace ct::gfx::detail
