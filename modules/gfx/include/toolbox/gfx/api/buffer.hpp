#pragma once
#include "toolbox/base/base.hpp"
#include <webgpu/webgpu_cpp.h>
#include <span>

namespace ct {

class Device;

enum class BufferType : u8 {
    Vertex  = 0,
    Index   = 1,
    Uniform = 2,
    Storage = 3,
};

struct BufferInfo {
    BufferType type{BufferType::Vertex};
    u64 size{0};
    bool mappable{false};
    const void* initialData{nullptr};

    [[nodiscard]] static constexpr BufferInfo Vertex(u64 size, const void* data = nullptr) noexcept {
        return {BufferType::Vertex, size, false, data};
    }

    [[nodiscard]] static constexpr BufferInfo Index(u64 size, const void* data = nullptr) noexcept {
        return {BufferType::Index, size, false, data};
    }

    [[nodiscard]] static constexpr BufferInfo Uniform(u64 size, const void* data = nullptr) noexcept {
        return {BufferType::Uniform, size, false, data};
    }

    [[nodiscard]] static constexpr BufferInfo Storage(u64 size, const void* data = nullptr) noexcept {
        return {BufferType::Storage, size, false, data};
    }

    //NOTE: mappable buffers get MapWrite | CopySrc usage for staging patterns
    [[nodiscard]] static constexpr BufferInfo Staging(u64 size) noexcept {
        return {BufferType::Vertex, size, true, nullptr};
    }
};

class Buffer {
public:
    ~Buffer();

    void Write(const void* data, u64 size, u64 offset = 0);

    template<typename T>
    void Write(const T& value, u64 offset = 0) {
        Write(&value, sizeof(T), offset);
    }

    template<typename T, std::size_t Extent>
    void Write(std::span<const T, Extent> data, u64 offset = 0) {
        Write(data.data(), data.size_bytes(), offset);
    }

    [[nodiscard]] BufferType GetType() const noexcept;
    [[nodiscard]] u64 GetSize() const noexcept;
    [[nodiscard]] bool IsMappable() const noexcept;
    [[nodiscard]] wgpu::Buffer GetHandle() const noexcept;

    //NOTE: caller must ensure device outlives this Buffer
    [[nodiscard]] static result<ref<Buffer>> Create(
        ref<Device> device, const BufferInfo& info) noexcept;

    template<typename T, std::size_t Extent>
    [[nodiscard]] static result<ref<Buffer>> Create(
        ref<Device> device, BufferType type, std::span<const T, Extent> data) noexcept {
        BufferInfo info{};
        info.type = type;
        info.size = data.size_bytes();
        info.initialData = data.data();
        return Create(device, info);
    }

private:
    Buffer() = default;
    bool Init(const Device& device, const BufferInfo& info);

    wgpu::Buffer mBuffer{nullptr};
    Device* mDevice{nullptr};
    BufferType mType{BufferType::Vertex};
    u64 mSize{0};
    bool mMappable{false};
};

} // namespace ct
