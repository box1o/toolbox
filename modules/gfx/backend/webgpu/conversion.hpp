#pragma once
#include <toolbox/gfx/api/texture.hpp>
#include <toolbox/gfx/api/swapchain.hpp>
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx::webgpu {

inline wgpu::TextureFormat ToWGPU(TextureFormat fmt) noexcept {
    switch (fmt) {
        case TextureFormat::RGBA8Unorm:           return wgpu::TextureFormat::RGBA8Unorm;
        case TextureFormat::RGBA8UnormSrgb:       return wgpu::TextureFormat::RGBA8UnormSrgb;
        case TextureFormat::BGRA8Unorm:           return wgpu::TextureFormat::BGRA8Unorm;
        case TextureFormat::BGRA8UnormSrgb:       return wgpu::TextureFormat::BGRA8UnormSrgb;
        case TextureFormat::Depth16Unorm:         return wgpu::TextureFormat::Depth16Unorm;
        case TextureFormat::Depth24Plus:          return wgpu::TextureFormat::Depth24Plus;
        case TextureFormat::Depth24PlusStencil8:  return wgpu::TextureFormat::Depth24PlusStencil8;
        case TextureFormat::Depth32Float:         return wgpu::TextureFormat::Depth32Float;
        case TextureFormat::Depth32FloatStencil8: return wgpu::TextureFormat::Depth32FloatStencil8;
        default:                                  return wgpu::TextureFormat::Undefined;
    }
}

inline wgpu::PresentMode ToWGPU(PresentMode pm) noexcept {
    switch (pm) {
        case PresentMode::Immediate: return wgpu::PresentMode::Immediate;
        case PresentMode::Mailbox:   return wgpu::PresentMode::Mailbox;
        case PresentMode::VSync:     return wgpu::PresentMode::Fifo;
        default:                     return wgpu::PresentMode::Fifo;
    }
}

} // namespace ct::gfx::webgpu
