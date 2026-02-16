#pragma once
#include "toolbox/base/base.hpp"
#include <string>
#include <webgpu/webgpu_cpp.h>

namespace ct {

inline std::string dsv(wgpu::StringView sv) {
    if (!sv.data || sv.length == 0) return {};
    return std::string(sv.data, sv.length);
}

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

enum class TextureDimension : u8 {
    D2,
    D3,
    Cube,
    Array2D,
};

enum class TextureUsageFlags : u8 {
    None = 0,
    Sampled = 1 << 0,
    Storage = 1 << 1,
    RenderTarget = 1 << 2,
    CopySrc = 1 << 3,
    CopyDst = 1 << 4,
};

constexpr TextureUsageFlags operator|(TextureUsageFlags a, TextureUsageFlags b) noexcept {
    return static_cast<TextureUsageFlags>(static_cast<u8>(a) | static_cast<u8>(b));
}
constexpr TextureUsageFlags operator&(TextureUsageFlags a, TextureUsageFlags b) noexcept {
    return static_cast<TextureUsageFlags>(static_cast<u8>(a) & static_cast<u8>(b));
}
constexpr bool HasFlag(TextureUsageFlags value, TextureUsageFlags flag) noexcept {
    return (static_cast<u8>(value) & static_cast<u8>(flag)) != 0;
}

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

inline wgpu::TextureFormat ToWGPU(TextureFormat fmt) {
    switch (fmt) {
    case TextureFormat::RGBA8Unorm:
        return wgpu::TextureFormat::RGBA8Unorm;
    case TextureFormat::RGBA8Snorm:
        return wgpu::TextureFormat::RGBA8Snorm;
    case TextureFormat::RGBA8UnormSrgb:
        return wgpu::TextureFormat::RGBA8UnormSrgb;
    case TextureFormat::BGRA8Unorm:
        return wgpu::TextureFormat::BGRA8Unorm;
    case TextureFormat::BGRA8UnormSrgb:
        return wgpu::TextureFormat::BGRA8UnormSrgb;
    case TextureFormat::R8Unorm:
        return wgpu::TextureFormat::R8Unorm;
    case TextureFormat::RG8Unorm:
        return wgpu::TextureFormat::RG8Unorm;
    case TextureFormat::RGBA16Float:
        return wgpu::TextureFormat::RGBA16Float;
    case TextureFormat::RGBA32Float:
        return wgpu::TextureFormat::RGBA32Float;
    case TextureFormat::R16Float:
        return wgpu::TextureFormat::R16Float;
    case TextureFormat::R32Float:
        return wgpu::TextureFormat::R32Float;
    case TextureFormat::RG16Float:
        return wgpu::TextureFormat::RG16Float;
    case TextureFormat::RG32Float:
        return wgpu::TextureFormat::RG32Float;
    case TextureFormat::Depth16Unorm:
        return wgpu::TextureFormat::Depth16Unorm;
    case TextureFormat::Depth24Plus:
        return wgpu::TextureFormat::Depth24Plus;
    case TextureFormat::Depth24PlusStencil8:
        return wgpu::TextureFormat::Depth24PlusStencil8;
    case TextureFormat::Depth32Float:
        return wgpu::TextureFormat::Depth32Float;
    case TextureFormat::Depth32FloatStencil8:
        return wgpu::TextureFormat::Depth32FloatStencil8;
    }
    return wgpu::TextureFormat::RGBA8Unorm;
}

inline wgpu::TextureUsage ToWGPUUsage(TextureUsageFlags flags) {
    wgpu::TextureUsage usage = wgpu::TextureUsage::None;
    if (HasFlag(flags, TextureUsageFlags::Sampled)) usage |= wgpu::TextureUsage::TextureBinding;
    if (HasFlag(flags, TextureUsageFlags::Storage)) usage |= wgpu::TextureUsage::StorageBinding;
    if (HasFlag(flags, TextureUsageFlags::RenderTarget))
        usage |= wgpu::TextureUsage::RenderAttachment;
    if (HasFlag(flags, TextureUsageFlags::CopySrc)) usage |= wgpu::TextureUsage::CopySrc;
    if (HasFlag(flags, TextureUsageFlags::CopyDst)) usage |= wgpu::TextureUsage::CopyDst;
    return usage;
}

inline wgpu::TextureDimension ToWGPUDimension(TextureDimension dim) {
    switch (dim) {
    case TextureDimension::D2:
    case TextureDimension::Cube:
    case TextureDimension::Array2D:
        return wgpu::TextureDimension::e2D;
    case TextureDimension::D3:
        return wgpu::TextureDimension::e3D;
    }
    return wgpu::TextureDimension::e2D;
}

inline wgpu::FilterMode ToWGPU(FilterMode mode) {
    switch (mode) {
    case FilterMode::Nearest:
        return wgpu::FilterMode::Nearest;
    case FilterMode::Linear:
        return wgpu::FilterMode::Linear;
    }
    return wgpu::FilterMode::Linear;
}

inline wgpu::MipmapFilterMode ToWGPUMipmap(FilterMode mode) {
    switch (mode) {
    case FilterMode::Nearest:
        return wgpu::MipmapFilterMode::Nearest;
    case FilterMode::Linear:
        return wgpu::MipmapFilterMode::Linear;
    }
    return wgpu::MipmapFilterMode::Linear;
}

inline wgpu::AddressMode ToWGPU(AddressMode mode) {
    switch (mode) {
    case AddressMode::Repeat:
        return wgpu::AddressMode::Repeat;
    case AddressMode::MirrorRepeat:
        return wgpu::AddressMode::MirrorRepeat;
    case AddressMode::ClampToEdge:
        return wgpu::AddressMode::ClampToEdge;
    }
    return wgpu::AddressMode::Repeat;
}

inline wgpu::CompareFunction ToWGPU(CompareFunction fn) {
    switch (fn) {
    case CompareFunction::Undefined:
        return wgpu::CompareFunction::Undefined;
    case CompareFunction::Never:
        return wgpu::CompareFunction::Never;
    case CompareFunction::Less:
        return wgpu::CompareFunction::Less;
    case CompareFunction::Equal:
        return wgpu::CompareFunction::Equal;
    case CompareFunction::LessEqual:
        return wgpu::CompareFunction::LessEqual;
    case CompareFunction::Greater:
        return wgpu::CompareFunction::Greater;
    case CompareFunction::NotEqual:
        return wgpu::CompareFunction::NotEqual;
    case CompareFunction::GreaterEqual:
        return wgpu::CompareFunction::GreaterEqual;
    case CompareFunction::Always:
        return wgpu::CompareFunction::Always;
    }
    return wgpu::CompareFunction::Undefined;
}

inline u32 TextureFormatBytesPerPixel(TextureFormat fmt) {
    switch (fmt) {
    case TextureFormat::R8Unorm:
        return 1;
    case TextureFormat::RG8Unorm:
        return 2;
    case TextureFormat::RGBA8Unorm:
    case TextureFormat::RGBA8Snorm:
    case TextureFormat::RGBA8UnormSrgb:
    case TextureFormat::BGRA8Unorm:
    case TextureFormat::BGRA8UnormSrgb:
        return 4;
    case TextureFormat::R16Float:
        return 2;
    case TextureFormat::RG16Float:
        return 4;
    case TextureFormat::R32Float:
        return 4;
    case TextureFormat::RGBA16Float:
        return 8;
    case TextureFormat::RG32Float:
        return 8;
    case TextureFormat::RGBA32Float:
        return 16;
    case TextureFormat::Depth16Unorm:
        return 2;
    case TextureFormat::Depth24Plus:
        return 4;
    case TextureFormat::Depth24PlusStencil8:
        return 4;
    case TextureFormat::Depth32Float:
        return 4;
    case TextureFormat::Depth32FloatStencil8:
        return 5;
    }
    return 4;
}

inline bool IsDepthFormat(TextureFormat fmt) {
    switch (fmt) {
    case TextureFormat::Depth16Unorm:
    case TextureFormat::Depth24Plus:
    case TextureFormat::Depth24PlusStencil8:
    case TextureFormat::Depth32Float:
    case TextureFormat::Depth32FloatStencil8:
        return true;
    default:
        return false;
    }
}

inline TextureFormat ChannelsToFormat(int channels) noexcept {
    switch (channels) {
    case 1:
        return TextureFormat::R8Unorm;
    case 2:
        return TextureFormat::RG8Unorm;
    case 4:
        return TextureFormat::RGBA8Unorm;
    default:
        return TextureFormat::RGBA8Unorm;
    }
}

} // namespace ct
