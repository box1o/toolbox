#pragma once
#include <toolbox/gfx/api/buffer.hpp>
#include <toolbox/gfx/api/device.hpp>
#include <toolbox/gfx/api/sampler.hpp>
#include <toolbox/gfx/api/texture.hpp>
#include <toolbox/gfx/api/shader.hpp>
#include <toolbox/gfx/api/pipeline.hpp>
#include <toolbox/gfx/api/vertex_layout.hpp>
#include <toolbox/gfx/api/bind_group_layout.hpp>
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx::detail {

constexpr wgpu::PowerPreference ToWGPU(PowerProfile p) noexcept {
    switch (p) {
    case PowerProfile::LowPower:        return wgpu::PowerPreference::LowPower;
    case PowerProfile::HighPerformance: return wgpu::PowerPreference::HighPerformance;
    default:                            return wgpu::PowerPreference::Undefined;
    }
}

constexpr std::string ToString(wgpu::StringView sv) noexcept {
    if (!sv.data || sv.length == 0) return {};
    return std::string(sv.data, sv.length);
}

// NOTE: Buffer
constexpr wgpu::BufferUsage ToWGPU(BufferUsageFlags usage) noexcept {
    wgpu::BufferUsage out = wgpu::BufferUsage::None;
    if (HasFlag(usage, BufferUsageFlags::Vertex))       out |= wgpu::BufferUsage::Vertex;
    if (HasFlag(usage, BufferUsageFlags::Index))        out |= wgpu::BufferUsage::Index;
    if (HasFlag(usage, BufferUsageFlags::Uniform))      out |= wgpu::BufferUsage::Uniform;
    if (HasFlag(usage, BufferUsageFlags::Storage))      out |= wgpu::BufferUsage::Storage;
    if (HasFlag(usage, BufferUsageFlags::CopySrc))      out |= wgpu::BufferUsage::CopySrc;
    if (HasFlag(usage, BufferUsageFlags::CopyDst))      out |= wgpu::BufferUsage::CopyDst;
    if (HasFlag(usage, BufferUsageFlags::MapRead))      out |= wgpu::BufferUsage::MapRead;
    if (HasFlag(usage, BufferUsageFlags::MapWrite))     out |= wgpu::BufferUsage::MapWrite;
    if (HasFlag(usage, BufferUsageFlags::Indirect))     out |= wgpu::BufferUsage::Indirect;
    if (HasFlag(usage, BufferUsageFlags::QueryResolve)) out |= wgpu::BufferUsage::QueryResolve;
    return out;
}

// NOTE: Texture
constexpr wgpu::TextureFormat ToWGPU(TextureFormat fmt) noexcept {
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

constexpr wgpu::TextureUsage ToWGPUUsage(TextureUsageFlags usage) noexcept {
    wgpu::TextureUsage out = wgpu::TextureUsage::None;
    if (HasFlag(usage, TextureUsageFlags::Sampled))      out |= wgpu::TextureUsage::TextureBinding;
    if (HasFlag(usage, TextureUsageFlags::Storage))      out |= wgpu::TextureUsage::StorageBinding;
    if (HasFlag(usage, TextureUsageFlags::RenderTarget)) out |= wgpu::TextureUsage::RenderAttachment;
    if (HasFlag(usage, TextureUsageFlags::CopySrc))      out |= wgpu::TextureUsage::CopySrc;
    if (HasFlag(usage, TextureUsageFlags::CopyDst))      out |= wgpu::TextureUsage::CopyDst;
    return out;
}

// NOTE: Sampler
constexpr wgpu::FilterMode ToWGPU(TextureFilter f) noexcept {
    switch (f) {
    case TextureFilter::Nearest: return wgpu::FilterMode::Nearest;
    case TextureFilter::Linear:  return wgpu::FilterMode::Linear;
    default:                     return wgpu::FilterMode::Nearest;
    }
}

constexpr wgpu::AddressMode ToWGPU(TextureWrap w) noexcept {
    switch (w) {
    case TextureWrap::Repeat:       return wgpu::AddressMode::Repeat;
    case TextureWrap::MirrorRepeat: return wgpu::AddressMode::MirrorRepeat;
    case TextureWrap::ClampToEdge:  return wgpu::AddressMode::ClampToEdge;
    default:                        return wgpu::AddressMode::Repeat;
    }
}

// NOTE: Vertex layout
constexpr wgpu::VertexFormat ToWGPU(VertexFormat fmt) noexcept {
    switch (fmt) {
    case VertexFormat::Float32:         return wgpu::VertexFormat::Float32;
    case VertexFormat::Float32x2:       return wgpu::VertexFormat::Float32x2;
    case VertexFormat::Float32x3:       return wgpu::VertexFormat::Float32x3;
    case VertexFormat::Float32x4:       return wgpu::VertexFormat::Float32x4;
    case VertexFormat::Sint32:          return wgpu::VertexFormat::Sint32;
    case VertexFormat::Sint32x2:        return wgpu::VertexFormat::Sint32x2;
    case VertexFormat::Sint32x3:        return wgpu::VertexFormat::Sint32x3;
    case VertexFormat::Sint32x4:        return wgpu::VertexFormat::Sint32x4;
    case VertexFormat::Uint32:          return wgpu::VertexFormat::Uint32;
    case VertexFormat::Uint32x2:        return wgpu::VertexFormat::Uint32x2;
    case VertexFormat::Uint32x3:        return wgpu::VertexFormat::Uint32x3;
    case VertexFormat::Uint32x4:        return wgpu::VertexFormat::Uint32x4;
    case VertexFormat::Sint16x2:        return wgpu::VertexFormat::Sint16x2;
    case VertexFormat::Sint16x4:        return wgpu::VertexFormat::Sint16x4;
    case VertexFormat::Uint16x2:        return wgpu::VertexFormat::Uint16x2;
    case VertexFormat::Uint16x4:        return wgpu::VertexFormat::Uint16x4;
    case VertexFormat::Snorm16x2:       return wgpu::VertexFormat::Snorm16x2;
    case VertexFormat::Snorm16x4:       return wgpu::VertexFormat::Snorm16x4;
    case VertexFormat::Unorm16x2:       return wgpu::VertexFormat::Unorm16x2;
    case VertexFormat::Unorm16x4:       return wgpu::VertexFormat::Unorm16x4;
    case VertexFormat::Sint8x2:         return wgpu::VertexFormat::Sint8x2;
    case VertexFormat::Sint8x4:         return wgpu::VertexFormat::Sint8x4;
    case VertexFormat::Uint8x2:         return wgpu::VertexFormat::Uint8x2;
    case VertexFormat::Uint8x4:         return wgpu::VertexFormat::Uint8x4;
    case VertexFormat::Snorm8x2:        return wgpu::VertexFormat::Snorm8x2;
    case VertexFormat::Snorm8x4:        return wgpu::VertexFormat::Snorm8x4;
    case VertexFormat::Unorm8x2:        return wgpu::VertexFormat::Unorm8x2;
    case VertexFormat::Unorm8x4:        return wgpu::VertexFormat::Unorm8x4;
    case VertexFormat::Unorm10_10_10_2: return wgpu::VertexFormat::Unorm10_10_10_2;
    default:                            return wgpu::VertexFormat::Float32;
    }
}

constexpr wgpu::VertexStepMode ToWGPU(VertexStepMode mode) noexcept {
    switch (mode) {
    case VertexStepMode::Vertex:   return wgpu::VertexStepMode::Vertex;
    case VertexStepMode::Instance: return wgpu::VertexStepMode::Instance;
    default:                       return wgpu::VertexStepMode::Vertex;
    }
}

// NOTE: Pipeline rasterizer
constexpr wgpu::FrontFace ToWGPU(FrontFace f) noexcept {
    switch (f) {
    case FrontFace::CCW: return wgpu::FrontFace::CCW;
    case FrontFace::CW:  return wgpu::FrontFace::CW;
    default:             return wgpu::FrontFace::CCW;
    }
}

constexpr wgpu::CullMode ToWGPU(CullMode c) noexcept {
    switch (c) {
    case CullMode::None:  return wgpu::CullMode::None;
    case CullMode::Front: return wgpu::CullMode::Front;
    case CullMode::Back:  return wgpu::CullMode::Back;
    default:              return wgpu::CullMode::None;
    }
}

constexpr wgpu::PrimitiveTopology ToWGPU(PrimitiveTopology t) noexcept {
    switch (t) {
    case PrimitiveTopology::TriangleList:  return wgpu::PrimitiveTopology::TriangleList;
    case PrimitiveTopology::TriangleStrip: return wgpu::PrimitiveTopology::TriangleStrip;
    case PrimitiveTopology::LineList:      return wgpu::PrimitiveTopology::LineList;
    case PrimitiveTopology::LineStrip:     return wgpu::PrimitiveTopology::LineStrip;
    case PrimitiveTopology::PointList:     return wgpu::PrimitiveTopology::PointList;
    default:                               return wgpu::PrimitiveTopology::TriangleList;
    }
}

// NOTE: Pipeline depth/stencil
constexpr wgpu::CompareFunction ToWGPU(CompareOp op) noexcept {
    switch (op) {
    case CompareOp::Never:        return wgpu::CompareFunction::Never;
    case CompareOp::Less:         return wgpu::CompareFunction::Less;
    case CompareOp::Equal:        return wgpu::CompareFunction::Equal;
    case CompareOp::LessEqual:    return wgpu::CompareFunction::LessEqual;
    case CompareOp::Greater:      return wgpu::CompareFunction::Greater;
    case CompareOp::NotEqual:     return wgpu::CompareFunction::NotEqual;
    case CompareOp::GreaterEqual: return wgpu::CompareFunction::GreaterEqual;
    case CompareOp::Always:       return wgpu::CompareFunction::Always;
    default:                      return wgpu::CompareFunction::Always;
    }
}

// NOTE: Pipeline blend
constexpr wgpu::BlendFactor ToWGPU(BlendFactor f) noexcept {
    switch (f) {
    case BlendFactor::Zero:             return wgpu::BlendFactor::Zero;
    case BlendFactor::One:              return wgpu::BlendFactor::One;
    case BlendFactor::SrcColor:         return wgpu::BlendFactor::Src;
    case BlendFactor::OneMinusSrcColor: return wgpu::BlendFactor::OneMinusSrc;
    case BlendFactor::DstColor:         return wgpu::BlendFactor::Dst;
    case BlendFactor::OneMinusDstColor: return wgpu::BlendFactor::OneMinusDst;
    case BlendFactor::SrcAlpha:         return wgpu::BlendFactor::SrcAlpha;
    case BlendFactor::OneMinusSrcAlpha: return wgpu::BlendFactor::OneMinusSrcAlpha;
    case BlendFactor::DstAlpha:         return wgpu::BlendFactor::DstAlpha;
    case BlendFactor::OneMinusDstAlpha: return wgpu::BlendFactor::OneMinusDstAlpha;
    default:                            return wgpu::BlendFactor::One;
    }
}

constexpr wgpu::BlendOperation ToWGPU(BlendOp op) noexcept {
    switch (op) {
    case BlendOp::Add:             return wgpu::BlendOperation::Add;
    case BlendOp::Subtract:        return wgpu::BlendOperation::Subtract;
    case BlendOp::ReverseSubtract: return wgpu::BlendOperation::ReverseSubtract;
    case BlendOp::Min:             return wgpu::BlendOperation::Min;
    case BlendOp::Max:             return wgpu::BlendOperation::Max;
    default:                       return wgpu::BlendOperation::Add;
    }
}

// NOTE: Bind group layout
constexpr wgpu::BufferBindingType ToWGPUBufferBinding(BindingType type) noexcept {
    switch (type) {
    case BindingType::UniformBuffer:         return wgpu::BufferBindingType::Uniform;
    case BindingType::StorageBuffer:         return wgpu::BufferBindingType::Storage;
    case BindingType::ReadOnlyStorageBuffer: return wgpu::BufferBindingType::ReadOnlyStorage;
    default:                                 return wgpu::BufferBindingType::Undefined;
    }
}

constexpr wgpu::TextureSampleType ToWGPU(TextureSampleType t) noexcept {
    switch (t) {
    case TextureSampleType::Float:             return wgpu::TextureSampleType::Float;
    case TextureSampleType::UnfilterableFloat: return wgpu::TextureSampleType::UnfilterableFloat;
    case TextureSampleType::Depth:             return wgpu::TextureSampleType::Depth;
    case TextureSampleType::Sint:              return wgpu::TextureSampleType::Sint;
    case TextureSampleType::Uint:              return wgpu::TextureSampleType::Uint;
    default:                                   return wgpu::TextureSampleType::Float;
    }
}

constexpr wgpu::TextureViewDimension ToWGPU(TextureViewDimension d) noexcept {
    switch (d) {
    case TextureViewDimension::e1D:       return wgpu::TextureViewDimension::e1D;
    case TextureViewDimension::e2D:       return wgpu::TextureViewDimension::e2D;
    case TextureViewDimension::e2DArray:  return wgpu::TextureViewDimension::e2DArray;
    case TextureViewDimension::e3D:       return wgpu::TextureViewDimension::e3D;
    case TextureViewDimension::Cube:      return wgpu::TextureViewDimension::Cube;
    case TextureViewDimension::CubeArray: return wgpu::TextureViewDimension::CubeArray;
    default:                              return wgpu::TextureViewDimension::e2D;
    }
}

constexpr wgpu::ShaderStage ToWGPUVisibility(u8 flags) noexcept {
    wgpu::ShaderStage out = wgpu::ShaderStage::None;
    if (flags & static_cast<u8>(ShaderStage::Vertex))   out |= wgpu::ShaderStage::Vertex;
    if (flags & static_cast<u8>(ShaderStage::Fragment)) out |= wgpu::ShaderStage::Fragment;
    if (flags & static_cast<u8>(ShaderStage::Compute))  out |= wgpu::ShaderStage::Compute;
    return out;
}

// clang-format off
constexpr std::string StageFlagsToString(ShaderStage flags) noexcept {
    std::string out;
    if (HasStage(flags, ShaderStage::Vertex))   {                                 out += "vertex";   }
    if (HasStage(flags, ShaderStage::Fragment)) { if (!out.empty()) out += " | "; out += "fragment"; }
    if (HasStage(flags, ShaderStage::Compute))  { if (!out.empty()) out += " | "; out += "compute";  }
    if (out.empty()) out = "none";
    return out;
}
// clang-format on

} // namespace ct::gfx::detail
