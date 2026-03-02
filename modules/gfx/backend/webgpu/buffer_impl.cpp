#include "buffer_impl.hpp"

#include "common.hpp"
#include "toolbox/gfx/api/device.hpp"

#include <limits>
#include <string>
#include <span>

namespace ct::gfx::webgpu {

BufferImpl::BufferImpl(ref<Device> device, const BufferDesc& desc)
    : mDesc(desc), mDevice(std::move(device)) {

    if (mDevice) {
        mWgpuDevice = *static_cast<wgpu::Device*>(mDevice->GetNativeDeviceHandle());
        mQueue      = *static_cast<wgpu::Queue*>(mDevice->GetNativeQueueHandle());
        mInstance   = *static_cast<wgpu::Instance*>(mDevice->GetNativeInstanceHandle());

        if (!mWgpuDevice) log::Error("Buffer: native device handle is null");
        if (!mQueue)      log::Error("Buffer: native queue handle is null");
        if (!mInstance)   log::Error("Buffer: native instance handle is null");
    }
}

result<void> BufferImpl::Initialize() noexcept {
    if (!mDevice)     return err(ErrorCode::INVALID_ARGUMENT, "Buffer: device is null");
    if (!mWgpuDevice) return err(ErrorCode::INVALID_STATE,    "Buffer: native device is null");
    if (!mQueue)      return err(ErrorCode::INVALID_STATE,    "Buffer: native queue is null");
    if (!mInstance)   return err(ErrorCode::INVALID_STATE,    "Buffer: native instance is null");

    if (mDesc.size == 0) {
        return err(ErrorCode::INVALID_ARGUMENT, "Buffer: size must be > 0");
    }

    // WebGPU is happiest when buffer sizes are aligned.
    mAllocatedSize = AlignUp(mDesc.size, 4);

    wgpu::BufferDescriptor bd{};
    bd.size = static_cast<size_t>(mAllocatedSize);
    bd.usage = detail::ToWGPU(mDesc.usage);
    bd.mappedAtCreation = false;

    mBuffer = mWgpuDevice.CreateBuffer(&bd);
    if (!mBuffer) {
        log::Error("Buffer: CreateBuffer failed");
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Buffer: CreateBuffer failed");
    }

    return ok();
}

result<void> BufferImpl::Update(u64 byteOffset, const void* data, u64 numBytes) noexcept {
    if (!mBuffer || !mQueue) {
        return err(ErrorCode::INVALID_STATE, "Buffer: not initialized");
    }
    if (!data) {
        return err(ErrorCode::INVALID_ARGUMENT, "Buffer: data is null");
    }
    if (numBytes == 0) {
        return err(ErrorCode::INVALID_ARGUMENT, "Buffer: numBytes is 0");
    }

    if (!HasFlag(mDesc.usage, BufferUsageFlags::CopyDst)) {
        return err(ErrorCode::INVALID_STATE, "Buffer buffer missing CopyDst usage");
    }

    // WebGPU requires offset and size to be multiples of 4 for WriteBuffer.
    if ((byteOffset % 4u) != 0u || (numBytes % 4u) != 0u) {
        return err(ErrorCode::INVALID_ARGUMENT,
            "Buffer: byteOffset and numBytes must be multiples of 4");
    }

    if (byteOffset + numBytes > mAllocatedSize) {
        return err(ErrorCode::OUT_OF_RANGE, "Buffer: write range exceeds buffer size");
    }

    mQueue.WriteBuffer(mBuffer, static_cast<size_t>(byteOffset), data, static_cast<size_t>(numBytes));
    return ok();
}

result<void*> BufferImpl::MapImpl(
    wgpu::MapMode mode,
    BufferUsageFlags requiredFlag,
    u64 byteOffset,
    u64 numBytes) noexcept {

    if (!mBuffer || !mInstance) {
        return err(ErrorCode::INVALID_STATE, "Buffer: not initialized");
    }

    if (!HasFlag(mDesc.usage, requiredFlag)) {
        const bool isWrite = (mode == wgpu::MapMode::Write);
        return err(ErrorCode::INVALID_STATE,
            isWrite ? "Buffer: buffer missing MapWrite usage"
                    : "Buffer:  buffer missing MapRead usage");
    }

    if ((byteOffset % 8u) != 0u) {
        return err(ErrorCode::INVALID_ARGUMENT, "BufferImpl::MapImpl: byteOffset must be 8-byte aligned");
    }

    if (byteOffset >= mAllocatedSize) {
        return err(ErrorCode::OUT_OF_RANGE, "BufferImpl::MapImpl: byteOffset >= buffer size");
    }

    if (numBytes == 0) {
        numBytes = mAllocatedSize - byteOffset; // map remaining allocation
    }

    if ((numBytes % 4u) != 0u) {
        return err(ErrorCode::INVALID_ARGUMENT, "BufferImpl::MapImpl: numBytes must be multiple of 4");
    }

    if (byteOffset + numBytes > mAllocatedSize) {
        return err(ErrorCode::OUT_OF_RANGE, "BufferImpl::MapImpl: mapped range exceeds buffer size");
    }

    wgpu::MapAsyncStatus mapStatus = wgpu::MapAsyncStatus::Error;
    std::string mapMessage;

    wgpu::Future future = mBuffer.MapAsync(
        mode,
        static_cast<size_t>(byteOffset),
        static_cast<size_t>(numBytes),
        wgpu::CallbackMode::WaitAnyOnly,
        [&mapStatus, &mapMessage](wgpu::MapAsyncStatus status, wgpu::StringView message) {
            mapStatus = status;
            if (status != wgpu::MapAsyncStatus::Success) {
                mapMessage.assign(message.data ? message.data : "", message.length);
            }
        });

    wgpu::FutureWaitInfo waitInfo{ future, false };

    wgpu::WaitStatus waitStatus = mInstance.WaitAny(
        1,
        &waitInfo,
        std::numeric_limits<uint64_t>::max());

    if (waitStatus != wgpu::WaitStatus::Success) {
        log::Error("BufferImpl::MapImpl: instance.WaitAny failed (status={})", static_cast<int>(waitStatus));
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "BufferImpl::MapImpl: WaitAny failed");
    }

    if (mapStatus != wgpu::MapAsyncStatus::Success) {
        log::Error("BufferImpl::MapImpl: MapAsync failed status={} msg={}",
            static_cast<int>(mapStatus), mapMessage);
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "BufferImpl::MapImpl: MapAsync did not succeed");
    }

    void* ptr = nullptr;
    if (mode == wgpu::MapMode::Write) {
        ptr = mBuffer.GetMappedRange(static_cast<size_t>(byteOffset), static_cast<size_t>(numBytes));
    } else {
        ptr = const_cast<void*>(
            mBuffer.GetConstMappedRange(static_cast<size_t>(byteOffset), static_cast<size_t>(numBytes)));
    }

    if (!ptr) {
        log::Error("BufferImpl::MapImpl: GetMappedRange returned null");
        mBuffer.Unmap();
        return err(ErrorCode::INVALID_STATE, "BufferImpl::MapImpl: GetMappedRange returned null");
    }

    return ok(ptr);
}

result<void*> BufferImpl::MapWrite(u64 byteOffset, u64 numBytes) noexcept {
    return MapImpl(wgpu::MapMode::Write, BufferUsageFlags::MapWrite, byteOffset, numBytes);
}

result<const void*> BufferImpl::MapRead(u64 byteOffset, u64 numBytes) noexcept {
    auto res = MapImpl(wgpu::MapMode::Read, BufferUsageFlags::MapRead, byteOffset, numBytes);
    if (!res) return err(res.error());
    return ok(static_cast<const void*>(res.value()));
}

void BufferImpl::Unmap() noexcept {
    if (!mBuffer) return;

    if (mBuffer.GetMapState() != wgpu::BufferMapState::Unmapped) {
        mBuffer.Unmap();
    }
}

} // namespace ct::gfx::webgpu
