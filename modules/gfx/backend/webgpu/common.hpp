#pragma once
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

} // namespace ct::gfx::detail
