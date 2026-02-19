#pragma once
#include <toolbox/base/base.hpp>

namespace ct::gfx {

enum class TextureDimension : u8 {
    D2,
    D3,
    Cube,
    Array2D,
};

enum class TextureFormat : u8 {
    RGBA8Unorm,
    RGBA8Snorm,
    RGBA8UnormSrgb,
    BGRA8Unorm,
    BGRA8UnormSrgb,
    R8Unorm,
    RG8Unorm,
    RGBA16Float,
    RGBA32Float,
    R16Float,
    R32Float,
    RG16Float,
    RG32Float,
    Depth16Unorm,
    Depth24Plus,
    Depth24PlusStencil8,
    Depth32Float,
    Depth32FloatStencil8,
};

enum class TextureUsageFlags : u32 {
    None = 0,
    Sampled = 1u << 0,
    Storage = 1u << 1,
    RenderTarget = 1u << 2,
    CopySrc = 1u << 3,
    CopyDst = 1u << 4,
};

[[nodiscard]] constexpr TextureUsageFlags operator|(
    TextureUsageFlags a, TextureUsageFlags b) noexcept {
    return static_cast<TextureUsageFlags>(static_cast<u32>(a) | static_cast<u32>(b));
}

[[nodiscard]] constexpr TextureUsageFlags operator&(
    TextureUsageFlags a, TextureUsageFlags b) noexcept {
    return static_cast<TextureUsageFlags>(static_cast<u32>(a) & static_cast<u32>(b));
}

constexpr TextureUsageFlags& operator|=(TextureUsageFlags& a, TextureUsageFlags b) noexcept {
    a = a | b;
    return a;
}

constexpr TextureUsageFlags& operator&=(TextureUsageFlags& a, TextureUsageFlags b) noexcept {
    a = a & b;
    return a;
}

[[nodiscard]] constexpr bool HasFlag(TextureUsageFlags value, TextureUsageFlags flag) noexcept {
    return (static_cast<u32>(value) & static_cast<u32>(flag)) != 0u;
}


// Used by render pass / buffer indexing APIs
enum class IndexFormat : u8 { U16, U32 };

} // namespace ct::gfx
