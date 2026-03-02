#pragma once
#include <toolbox/base/base.hpp>
#include <span>

namespace ct::gfx {

enum class BufferType : u8 {
    Vertex = 0,
    Index = 1,
    Uniform = 2,
    Storage = 3,
    Staging = 4,
};

enum class BufferUsageFlags : u32 {
    None = 0,

    Vertex = 1u << 0,
    Index = 1u << 1,
    Uniform = 1u << 2,
    Storage = 1u << 3,

    CopySrc = 1u << 4,
    CopyDst = 1u << 5,

    MapRead = 1u << 6,
    MapWrite = 1u << 7,
    Indirect = 1u << 8,
    QueryResolve = 1u << 9,
};

// clang-format off
[[nodiscard]] constexpr BufferUsageFlags operator|(BufferUsageFlags a, BufferUsageFlags b) noexcept { return static_cast<BufferUsageFlags>(static_cast<u32>(a) | static_cast<u32>(b)); }
[[nodiscard]] constexpr BufferUsageFlags operator&(BufferUsageFlags a, BufferUsageFlags b) noexcept { return static_cast<BufferUsageFlags>(static_cast<u32>(a) & static_cast<u32>(b)); } constexpr BufferUsageFlags& operator|=(BufferUsageFlags& a, BufferUsageFlags b) noexcept { a = a | b; return a; }
[[nodiscard]] constexpr bool HasFlag(BufferUsageFlags v, BufferUsageFlags f) noexcept { return (static_cast<u32>(v) & static_cast<u32>(f)) != 0u; }
[[nodiscard]] constexpr BufferUsageFlags DefaultUsageFor(BufferType type) noexcept {
    switch (type) {
        case BufferType::Vertex:  return BufferUsageFlags::Vertex  | BufferUsageFlags::CopyDst;
        case BufferType::Index:   return BufferUsageFlags::Index   | BufferUsageFlags::CopyDst;
        case BufferType::Uniform: return BufferUsageFlags::Uniform | BufferUsageFlags::CopyDst;
        case BufferType::Storage: return BufferUsageFlags::Storage | BufferUsageFlags::CopyDst;
        case BufferType::Staging: return BufferUsageFlags::CopySrc | BufferUsageFlags::CopyDst | BufferUsageFlags::MapWrite; 
        default:                  return BufferUsageFlags::None;
    }
}
// clang-format on

struct BufferDesc {
    BufferType type{BufferType::Vertex};
    BufferUsageFlags usage{BufferUsageFlags::None};
    u64 size{0};
};

// fwd
class Device;

class Buffer {
public:
    virtual ~Buffer() = default;

    [[nodiscard]] virtual u64 GetSize() const noexcept = 0;
    [[nodiscard]] virtual BufferType GetType() const noexcept = 0;
    [[nodiscard]] virtual BufferUsageFlags GetUsage() const noexcept = 0;
    [[nodiscard]] virtual void* GetNativeBufferHandle() noexcept = 0;


    [[nodiscard]] virtual result<void*> MapWrite(u64 byteOffset = 0, u64 numBytes = 0) noexcept = 0;
    [[nodiscard]] virtual result<const void*> MapRead( u64 byteOffset = 0, u64 numBytes = 0) noexcept = 0;
    virtual void Unmap() noexcept = 0;

    virtual result<void> Update(u64 byteOffset, const void* data, u64 numBytes) noexcept = 0;

    template <class T> result<void> Update(u64 byteOffset, std::span<const T> src) noexcept {
        return Update(byteOffset, src.data(), static_cast<u64>(src.size_bytes()));
    }

    [[nodiscard]] static result<ref<Buffer>> Create(
        ref<Device> device, const BufferDesc& desc = {}) noexcept;

    // Convenience creators
    // clang-format off
    [[nodiscard]] static result<ref<Buffer>> CreateVertexBuffer( ref<Device> device, u64 size) noexcept { BufferDesc desc{}; desc.type = BufferType::Vertex; desc.usage = DefaultUsageFor(desc.type); desc.size = size; return Create(std::move(device), desc); }
    [[nodiscard]] static result<ref<Buffer>> CreateIndexBuffer( ref<Device> device, u64 size) noexcept { BufferDesc desc{}; desc.type = BufferType::Index; desc.usage = DefaultUsageFor(desc.type); desc.size = size; return Create(std::move(device), desc); }
    [[nodiscard]] static result<ref<Buffer>> CreateUniformBuffer( ref<Device> device, u64 size) noexcept { BufferDesc desc{}; desc.type = BufferType::Uniform; desc.usage = DefaultUsageFor(desc.type); desc.size = size; return Create(std::move(device), desc); }
    [[nodiscard]] static result<ref<Buffer>> CreateStorageBuffer( ref<Device> device, u64 size) noexcept { BufferDesc desc{}; desc.type = BufferType::Storage; desc.usage = DefaultUsageFor(desc.type); desc.size = size; return Create(std::move(device), desc); }
    // Staging (upload) buffer: CPU -> GPU
    [[nodiscard]] static result<ref<Buffer>> CreateUploadBuffer( ref<Device> device, u64 size) noexcept { BufferDesc desc{}; desc.type = BufferType::Staging; desc.usage = BufferUsageFlags::CopySrc | BufferUsageFlags::MapWrite; desc.size = size; return Create(std::move(device), desc); }
    // Readback buffer: GPU -> CPU
    [[nodiscard]] static result<ref<Buffer>> CreateReadbackBuffer( ref<Device> device, u64 size) noexcept { BufferDesc desc{}; desc.type = BufferType::Staging; desc.usage = BufferUsageFlags::CopyDst | BufferUsageFlags::MapRead; desc.size = size; return Create(std::move(device), desc); }
    //clang-format on

protected:
    Buffer() = default;
    virtual result<void> Initialize() noexcept = 0;
};

} // namespace ct::gfx
