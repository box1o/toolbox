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


// clang-format off
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

// clang-format off



} // namespace ct::gfx
