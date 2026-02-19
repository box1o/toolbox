#pragma once
#include <string>
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx {
static inline std::string dsv(wgpu::StringView sv) {
    if (!sv.data || sv.length == 0) return {};
    return std::string(sv.data, sv.length);
}

struct DebugConfig {
    bool verboseErrors{false};
    bool deviceLostLogs{true};
};

} // namespace ct::gfx
