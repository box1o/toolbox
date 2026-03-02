#pragma once
#include <toolbox/base/base.hpp>

#include <string>

#include <webgpu/webgpu_cpp.h>

#include "toolbox/gfx/api/swapchain.hpp"

namespace ct::gfx {
struct AdapterInfo {
    std::string vendor{};
    std::string architecture{};
    std::string device{};
    std::string description{};
    u32 backendType{0};
    u32 adapterType{0};
};

struct Limits {
    u32 maxVertexAttributes{0};
    u32 maxColorAttachments{0};
    u32 maxTextureDimension2D{0};
    u64 maxBufferSize{0};
    u32 maxBindGroups{0};
};



} // namespace ct::gfx
