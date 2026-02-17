#pragma once
#include "toolbox/base/base.hpp"
#include <vector>
#include <webgpu/webgpu_cpp.h>

namespace ct {

enum class VertexFormat : u8 {
    Float,
    Float2,
    Float3,
    Float4,
    Int,
    Int2,
    Int3,
    Int4,
    UInt,
    UInt2,
    UInt3,
    UInt4,
    Unorm8x2,
    Unorm8x4,
    Snorm8x2,
    Snorm8x4,
};

enum class VertexStepMode : u8 {
    PerVertex = 0,
    PerInstance = 1,
};

struct VertexAttribute {
    u32 location{0};
    VertexFormat format{VertexFormat::Float3};
    u32 offset{0};
};

struct VertexBufferBinding {
    u32 stride{0};
    VertexStepMode stepMode{VertexStepMode::PerVertex};
    std::vector<VertexAttribute> attributes{};
};

struct VertexLayout {
    std::vector<VertexBufferBinding> buffers{};

    // NOTE: builder pattern for inline construction
    VertexLayout& AddBuffer(u32 stride, VertexStepMode stepMode = VertexStepMode::PerVertex) {
        buffers.push_back({stride, stepMode, {}});
        return *this;
    }

    VertexLayout& AddAttribute(u32 location, VertexFormat format, u32 offset) {
        buffers.back().attributes.push_back({location, format, offset});
        return *this;
    }

    // NOTE: convenience — auto-computes stride from sequential attributes
    template <typename T>
    static VertexLayout Interleaved(std::initializer_list<std::pair<u32, VertexFormat>> attrs) {
        VertexLayout layout;
        layout.AddBuffer(sizeof(T));
        u32 offset = 0;
        for (const auto& [location, format] : attrs) {
            layout.AddAttribute(location, format, offset);
            offset += VertexFormatSize(format);
        }
        return layout;
    }

    [[nodiscard]] static constexpr u32 VertexFormatSize(VertexFormat fmt) noexcept {
        switch (fmt) {
        case VertexFormat::Float:
            return 4;
        case VertexFormat::Float2:
            return 8;
        case VertexFormat::Float3:
            return 12;
        case VertexFormat::Float4:
            return 16;
        case VertexFormat::Int:
            return 4;
        case VertexFormat::Int2:
            return 8;
        case VertexFormat::Int3:
            return 12;
        case VertexFormat::Int4:
            return 16;
        case VertexFormat::UInt:
            return 4;
        case VertexFormat::UInt2:
            return 8;
        case VertexFormat::UInt3:
            return 12;
        case VertexFormat::UInt4:
            return 16;
        case VertexFormat::Unorm8x2:
            return 2;
        case VertexFormat::Unorm8x4:
            return 4;
        case VertexFormat::Snorm8x2:
            return 2;
        case VertexFormat::Snorm8x4:
            return 4;
        }
        return 0;
    }
};

// NOTE: conversion used internally by RenderPipeline when building descriptors
inline wgpu::VertexFormat ToWGPU(VertexFormat fmt) {
    switch (fmt) {
    case VertexFormat::Float:
        return wgpu::VertexFormat::Float32;
    case VertexFormat::Float2:
        return wgpu::VertexFormat::Float32x2;
    case VertexFormat::Float3:
        return wgpu::VertexFormat::Float32x3;
    case VertexFormat::Float4:
        return wgpu::VertexFormat::Float32x4;
    case VertexFormat::Int:
        return wgpu::VertexFormat::Sint32;
    case VertexFormat::Int2:
        return wgpu::VertexFormat::Sint32x2;
    case VertexFormat::Int3:
        return wgpu::VertexFormat::Sint32x3;
    case VertexFormat::Int4:
        return wgpu::VertexFormat::Sint32x4;
    case VertexFormat::UInt:
        return wgpu::VertexFormat::Uint32;
    case VertexFormat::UInt2:
        return wgpu::VertexFormat::Uint32x2;
    case VertexFormat::UInt3:
        return wgpu::VertexFormat::Uint32x3;
    case VertexFormat::UInt4:
        return wgpu::VertexFormat::Uint32x4;
    case VertexFormat::Unorm8x2:
        return wgpu::VertexFormat::Unorm8x2;
    case VertexFormat::Unorm8x4:
        return wgpu::VertexFormat::Unorm8x4;
    case VertexFormat::Snorm8x2:
        return wgpu::VertexFormat::Snorm8x2;
    case VertexFormat::Snorm8x4:
        return wgpu::VertexFormat::Snorm8x4;
    }
    return wgpu::VertexFormat::Float32x3;
}

inline wgpu::VertexStepMode ToWGPU(VertexStepMode mode) {
    switch (mode) {
    case VertexStepMode::PerVertex:
        return wgpu::VertexStepMode::Vertex;
    case VertexStepMode::PerInstance:
        return wgpu::VertexStepMode::Instance;
    }
    return wgpu::VertexStepMode::Vertex;
}

} // namespace ct
