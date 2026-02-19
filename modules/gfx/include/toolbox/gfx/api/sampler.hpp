#pragma once
#include "toolbox/base/base.hpp"

namespace ct::gfx {
// ---- Sampler ----
enum class FilterMode : u8 {
    Nearest,
    Linear,
};

enum class AddressMode : u8 {
    Repeat,
    MirrorRepeat,
    ClampToEdge,
};

enum class CompareFunction : u8 {
    Undefined,
    Never,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always,
};

}
