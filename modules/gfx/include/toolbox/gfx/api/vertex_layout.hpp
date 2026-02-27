#pragma once
#include <toolbox/base/base.hpp>
#include <vector>

namespace ct::gfx {

enum class VertexFormat : u8 {
    Float32,
    Float32x2,
    Float32x3,
    Float32x4,
    Sint32,
    Sint32x2,
    Sint32x3,
    Sint32x4,
    Uint32,
    Uint32x2,
    Uint32x3,
    Uint32x4,
    Sint16x2,
    Sint16x4,
    Uint16x2,
    Uint16x4,
    Snorm16x2,
    Snorm16x4,
    Unorm16x2,
    Unorm16x4,
    Sint8x2,
    Sint8x4,
    Uint8x2,
    Uint8x4,
    Snorm8x2,
    Snorm8x4,
    Unorm8x2,
    Unorm8x4,
    Unorm10_10_10_2,
};

[[nodiscard]] constexpr u32 VertexFormatSize(VertexFormat fmt) noexcept {
    switch (fmt) {
    case VertexFormat::Float32:         return 4;
    case VertexFormat::Float32x2:       return 8;
    case VertexFormat::Float32x3:       return 12;
    case VertexFormat::Float32x4:       return 16;
    case VertexFormat::Sint32:          return 4;
    case VertexFormat::Sint32x2:        return 8;
    case VertexFormat::Sint32x3:        return 12;
    case VertexFormat::Sint32x4:        return 16;
    case VertexFormat::Uint32:          return 4;
    case VertexFormat::Uint32x2:        return 8;
    case VertexFormat::Uint32x3:        return 12;
    case VertexFormat::Uint32x4:        return 16;
    case VertexFormat::Sint16x2:        return 4;
    case VertexFormat::Sint16x4:        return 8;
    case VertexFormat::Uint16x2:        return 4;
    case VertexFormat::Uint16x4:        return 8;
    case VertexFormat::Snorm16x2:       return 4;
    case VertexFormat::Snorm16x4:       return 8;
    case VertexFormat::Unorm16x2:       return 4;
    case VertexFormat::Unorm16x4:       return 8;
    case VertexFormat::Sint8x2:         return 2;
    case VertexFormat::Sint8x4:         return 4;
    case VertexFormat::Uint8x2:         return 2;
    case VertexFormat::Uint8x4:         return 4;
    case VertexFormat::Snorm8x2:        return 2;
    case VertexFormat::Snorm8x4:        return 4;
    case VertexFormat::Unorm8x2:        return 2;
    case VertexFormat::Unorm8x4:        return 4;
    case VertexFormat::Unorm10_10_10_2: return 4;
    default:                            return 0;
    }
}

[[nodiscard]] constexpr u32 VertexFormatComponentCount(VertexFormat fmt) noexcept {
    switch (fmt) {
    case VertexFormat::Float32:
    case VertexFormat::Sint32:
    case VertexFormat::Uint32:          return 1;
    case VertexFormat::Float32x2:
    case VertexFormat::Sint32x2:
    case VertexFormat::Uint32x2:
    case VertexFormat::Sint16x2:
    case VertexFormat::Uint16x2:
    case VertexFormat::Snorm16x2:
    case VertexFormat::Unorm16x2:
    case VertexFormat::Sint8x2:
    case VertexFormat::Uint8x2:
    case VertexFormat::Snorm8x2:
    case VertexFormat::Unorm8x2:        return 2;
    case VertexFormat::Float32x3:
    case VertexFormat::Sint32x3:
    case VertexFormat::Uint32x3:        return 3;
    case VertexFormat::Float32x4:
    case VertexFormat::Sint32x4:
    case VertexFormat::Uint32x4:
    case VertexFormat::Sint16x4:
    case VertexFormat::Uint16x4:
    case VertexFormat::Snorm16x4:
    case VertexFormat::Unorm16x4:
    case VertexFormat::Sint8x4:
    case VertexFormat::Uint8x4:
    case VertexFormat::Snorm8x4:
    case VertexFormat::Unorm8x4:
    case VertexFormat::Unorm10_10_10_2: return 4;
    default:                            return 0;
    }
}

enum class VertexStepMode : u8 {
    Vertex,
    Instance,
};

struct VertexAttribute {
    u32 shaderLocation{0};
    VertexFormat format{VertexFormat::Float32};
    u64 offset{0};
};

struct VertexBufferLayout {
    u64 stride{0};
    VertexStepMode stepMode{VertexStepMode::Vertex};
    std::vector<VertexAttribute> attributes{};
};

class VertexLayout {
public:
    [[nodiscard]] const std::vector<VertexBufferLayout>& GetBuffers() const noexcept { return mBuffers; }
    [[nodiscard]] u32 GetTotalAttributeCount() const noexcept;

    class BufferBuilder;

    class Builder {
    public:
        Builder() = default;

        BufferBuilder AddBuffer(VertexStepMode stepMode = VertexStepMode::Vertex);
        [[nodiscard]] VertexLayout Build() noexcept;

    private:
        friend class BufferBuilder;
        std::vector<VertexBufferLayout> mBuffers{};
    };

    class BufferBuilder {
    public:
        BufferBuilder(Builder& parent, VertexStepMode stepMode);

        BufferBuilder& Attribute(u32 shaderLocation, VertexFormat format);
        BufferBuilder& AttributeAt(u32 shaderLocation, VertexFormat format, u64 manualOffset);

        BufferBuilder AddBuffer(VertexStepMode stepMode = VertexStepMode::Vertex);
        [[nodiscard]] VertexLayout Build() noexcept;

    private:
        void Finalize() noexcept;

        Builder& mParent;
        VertexBufferLayout mCurrent{};
        u64 mRunningOffset{0};
        bool mFinalized{false};
    };

    [[nodiscard]] static Builder Create() noexcept { return Builder{}; }

private:
    std::vector<VertexBufferLayout> mBuffers{};
};

} // namespace ct::gfx
