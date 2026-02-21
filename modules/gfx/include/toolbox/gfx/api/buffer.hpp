#pragma once
#include <toolbox/base/base.hpp>

namespace ct::gfx {

enum class BufferUsageFlags : u32 {
    None   = 0,
    Vertex = 1u << 0,
    Index  = 1u << 1,
    Uniform= 1u << 2,
    Storage= 1u << 3,
    CopySrc= 1u << 4,
    CopyDst= 1u << 5,
};

[[nodiscard]] constexpr BufferUsageFlags operator|(BufferUsageFlags a, BufferUsageFlags b) noexcept {
    return (BufferUsageFlags)((u32)a | (u32)b);
}
[[nodiscard]] constexpr BufferUsageFlags operator&(BufferUsageFlags a, BufferUsageFlags b) noexcept {
    return (BufferUsageFlags)((u32)a & (u32)b);
}
constexpr BufferUsageFlags& operator|=(BufferUsageFlags& a, BufferUsageFlags b) noexcept { a = a | b; return a; }
[[nodiscard]] constexpr bool HasFlag(BufferUsageFlags v, BufferUsageFlags f) noexcept {
    return ((u32)v & (u32)f) != 0u;
}

struct BufferDesc {
    u64 size{0};
    BufferUsageFlags usage{BufferUsageFlags::None};
    std::string debugName{"Buffer"};
};

class Device;

class Buffer {
public:
    virtual ~Buffer() = default;

    [[nodiscard]] virtual u64 GetSize() const noexcept = 0;
    [[nodiscard]] virtual void* GetNativeBuffer() const noexcept = 0;

    [[nodiscard]] virtual result<void> Update(const void* data, u64 size, u64 offset = 0) noexcept = 0;

    template<class T>
    [[nodiscard]] result<void> Update(const T& v, u64 offset = 0) noexcept {
        return Update(&v, (u64)sizeof(T), offset);
    }

    [[nodiscard]] static result<ref<Buffer>> Create(ref<Device> device, const BufferDesc& desc) noexcept;

    template<class T>
    [[nodiscard]] static result<ref<Buffer>> CreateUniform(ref<Device> device, const std::string& name = "UniformBuffer") noexcept {
        BufferDesc d{};
        d.size = (u64)sizeof(T);
        d.usage = BufferUsageFlags::Uniform | BufferUsageFlags::CopyDst;
        d.debugName = name;
        return Create(device, d);
    }

protected:
    Buffer() = default;
};

} // namespace ct::gfx
