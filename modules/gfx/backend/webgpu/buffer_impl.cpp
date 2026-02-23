#include "buffer_impl.hpp"
#include "device_impl.hpp"

#include <toolbox/base/logger/logger.hpp>

namespace ct::gfx::webgpu {

wgpu::BufferUsage BufferImpl::ToWGPUUsage(BufferUsageFlags usage) noexcept {
    wgpu::BufferUsage out = wgpu::BufferUsage::None;

    if (HasFlag(usage, BufferUsageFlags::Vertex)) out |= wgpu::BufferUsage::Vertex;
    if (HasFlag(usage, BufferUsageFlags::Index)) out |= wgpu::BufferUsage::Index;
    if (HasFlag(usage, BufferUsageFlags::Uniform)) out |= wgpu::BufferUsage::Uniform;
    if (HasFlag(usage, BufferUsageFlags::Storage)) out |= wgpu::BufferUsage::Storage;
    if (HasFlag(usage, BufferUsageFlags::CopySrc)) out |= wgpu::BufferUsage::CopySrc;
    if (HasFlag(usage, BufferUsageFlags::CopyDst)) out |= wgpu::BufferUsage::CopyDst;

    return out;
}

result<void> BufferImpl::Initialize(ref<Device> device, const BufferDesc& desc) noexcept {
    if (!device) return err(ErrorCode::INVALID_ARGUMENT, "BufferImpl: device null");
    if (desc.size == 0) {
        log::Error("BufferImpl: size=0");
        return err(ErrorCode::INVALID_ARGUMENT, "BufferImpl: size=0");
    }

    auto* dev = dynamic_cast<DeviceImpl*>(device.get());
    if (!dev) {
        log::Error("BufferImpl: device is not WebGPU device");
        return err(ErrorCode::INVALID_ARGUMENT, "BufferImpl: device is not WebGPU device");
    }

    mDevice = dev->DeviceHandle();
    mQueue = dev->QueueHandle();
    if (!mDevice || !mQueue)
        return err(ErrorCode::INVALID_STATE, "BufferImpl: device or queue not initialized");

    mSize = desc.size;
    mUsage = desc.usage;

    if (!HasFlag(mUsage, BufferUsageFlags::CopyDst)) {
        mUsage |= BufferUsageFlags::CopyDst;
    }

    wgpu::BufferDescriptor bd{};
    bd.size = mSize;
    bd.usage = ToWGPUUsage(mUsage);
    if (!desc.debugName.empty()) bd.label = desc.debugName.c_str();

    mBuffer = mDevice.CreateBuffer(&bd);
    if (!mBuffer) {
        log::Error("BufferImpl: failed to create buffer");
        return err(
            ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "BufferImpl: failed to create buffer");
    }
    return ok();
}

result<void> BufferImpl::Update(const void* data, u64 size, u64 offset) noexcept {
    if (!mBuffer || !mQueue) {
        return err(ErrorCode::INVALID_STATE, "Buffer::Update: not initialized");
    }
    if (!data || size == 0) {
        return err(ErrorCode::INVALID_ARGUMENT, "Buffer::Update: invalid data/size");
    }
    if (offset + size > mSize) {
        return err(ErrorCode::OUT_OF_RANGE, "Buffer::Update: out of range");
    }
    if (!HasFlag(mUsage, BufferUsageFlags::CopyDst)) {
        return err(ErrorCode::INVALID_STATE, "Buffer::Update: missing CopyDst usage");
    }

    mQueue.WriteBuffer(mBuffer, offset, data, size);
    return ok();
}

} // namespace ct::gfx::webgpu
