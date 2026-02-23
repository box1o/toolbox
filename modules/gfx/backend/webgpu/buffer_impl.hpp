#pragma once
#include <toolbox/gfx/api/buffer.hpp>

#include <webgpu/webgpu_cpp.h>

namespace ct::gfx {
class Device;
}

namespace ct::gfx::webgpu {

class BufferImpl final : public Buffer {
public:
    BufferImpl() = default;
    ~BufferImpl() override = default;

    result<void> Initialize(ref<Device> device, const BufferDesc& desc) noexcept;

    [[nodiscard]] u64 GetSize() const noexcept override { return mSize; }
    [[nodiscard]] void* GetNativeBuffer() const noexcept override { return (void*)mBuffer.Get(); }

    [[nodiscard]] result<void> Update(const void* data, u64 size, u64 offset) noexcept override;

    [[nodiscard]] const wgpu::Buffer& Handle() const noexcept { return mBuffer; }

private:
    static wgpu::BufferUsage ToWGPUUsage(BufferUsageFlags usage) noexcept;

private:
    wgpu::Device mDevice;
    wgpu::Queue  mQueue;
    wgpu::Buffer mBuffer;

    u64 mSize{0};
    BufferUsageFlags mUsage{BufferUsageFlags::None};
};

} // namespace ct::gfx::webgpu
