#pragma once

#include <toolbox/gfx/api/buffer.hpp>
#include <webgpu/webgpu_cpp.h>


namespace ct::gfx::webgpu {

class BufferImpl final : public Buffer {
public:
    explicit BufferImpl(ref<Device> device, const BufferDesc& desc);
    ~BufferImpl() override = default;

    // Public API returns the requested/logical size (not the padded allocation).
    [[nodiscard]] u64 GetSize() const noexcept override { return mDesc.size; }
    [[nodiscard]] BufferType GetType() const noexcept override { return mDesc.type; }
    [[nodiscard]] BufferUsageFlags GetUsage() const noexcept override { return mDesc.usage; }

    // Native handle: pointer to wgpu::Buffer wrapper object.
    [[nodiscard]] void* GetNativeBufferHandle() noexcept override {
        return static_cast<void*>(&mBuffer);
    }

    [[nodiscard]] result<void*> MapWrite(u64 byteOffset = 0, u64 numBytes = 0) noexcept override;
    [[nodiscard]] result<const void*> MapRead(
        u64 byteOffset = 0, u64 numBytes = 0) noexcept override;
    void Unmap() noexcept override;

    result<void> Update(u64 byteOffset, const void* data, u64 numBytes) noexcept override;

    result<void> Initialize() noexcept override;
private:
    static constexpr u64 AlignUp(u64 v, u64 a) noexcept { return (v + (a - 1u)) & ~(a - 1u); }

    [[nodiscard]] result<void*> MapImpl(
        wgpu::MapMode mode, BufferUsageFlags requiredFlag, u64 byteOffset, u64 numBytes) noexcept;

private:
    BufferDesc mDesc{};
    ref<Device> mDevice{nullptr};

    wgpu::Instance mInstance{nullptr};
    wgpu::Device mWgpuDevice{nullptr};
    wgpu::Queue mQueue{nullptr};
    wgpu::Buffer mBuffer{nullptr};

    u64 mAllocatedSize{0}; // actual buffer allocation (aligned)
};

} // namespace ct::gfx::webgpu
