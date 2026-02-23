#pragma once
#include <toolbox/base/base.hpp>

namespace ct::gfx {

enum class TextureFormat : u8 {
    Undefined = 0,

    RGBA8Unorm,
    RGBA8UnormSrgb,
    BGRA8Unorm,
    BGRA8UnormSrgb,

    Depth16Unorm,
    Depth24Plus,
    Depth24PlusStencil8,
    Depth32Float,
    Depth32FloatStencil8,
};
enum class IndexFormat : u8 { U16, U32 };

} // namespace ct::gfx
