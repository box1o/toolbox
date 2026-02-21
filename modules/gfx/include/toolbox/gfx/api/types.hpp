#pragma once
#include <toolbox/base/base.hpp>

namespace ct::gfx {

enum class Backend : u8 {
    Auto = 0,
    WebGPU,
    Vulkan,
    Metal,
    D3D12,
};

enum class PowerPreference : u8 {
    LowPower,
    HighPerformance,
};

enum class QueueType : u8 {
    Graphics = 0,
    Compute,
    Transfer,
};

} // namespace ct::gfx
