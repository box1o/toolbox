#pragma once
#include "toolbox/gfx/api/texture.hpp"
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx {

inline constexpr wgpu::TextureFormat ToWGPUTextureFormat(TextureFormat format) noexcept {
    switch (format) {
    case TextureFormat::RGBA8Unorm: return wgpu::TextureFormat::RGBA8Unorm;
    case TextureFormat::RGBA8Snorm: return wgpu::TextureFormat::RGBA8Snorm;
    case TextureFormat::RGBA8UnormSrgb: return wgpu::TextureFormat::RGBA8UnormSrgb;
    case TextureFormat::BGRA8Unorm: return wgpu::TextureFormat::BGRA8Unorm;
    case TextureFormat::BGRA8UnormSrgb: return wgpu::TextureFormat::BGRA8UnormSrgb;
    case TextureFormat::R8Unorm: return wgpu::TextureFormat::R8Unorm;
    case TextureFormat::RG8Unorm: return wgpu::TextureFormat::RG8Unorm;
    case TextureFormat::RGBA16Float: return wgpu::TextureFormat::RGBA16Float;
    case TextureFormat::RGBA32Float: return wgpu::TextureFormat::RGBA32Float;
    case TextureFormat::R16Float: return wgpu::TextureFormat::R16Float;
    case TextureFormat::R32Float: return wgpu::TextureFormat::R32Float;
    case TextureFormat::RG16Float: return wgpu::TextureFormat::RG16Float;
    case TextureFormat::RG32Float: return wgpu::TextureFormat::RG32Float;
    case TextureFormat::Depth16Unorm: return wgpu::TextureFormat::Depth16Unorm;
    case TextureFormat::Depth24Plus: return wgpu::TextureFormat::Depth24Plus;
    case TextureFormat::Depth24PlusStencil8: return wgpu::TextureFormat::Depth24PlusStencil8;
    case TextureFormat::Depth32Float: return wgpu::TextureFormat::Depth32Float;
    case TextureFormat::Depth32FloatStencil8: return wgpu::TextureFormat::Depth32FloatStencil8;
    default: return wgpu::TextureFormat::Undefined; // default fallback
    }
}

} // namespace ct::gfx
