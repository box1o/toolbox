#include "buffer_impl.hpp"

#include "common.hpp"
#include "toolbox/gfx/api/device.hpp"

#include <limits>
#include <string>

namespace ct::gfx::webgpu {

BufferImpl::BufferImpl(ref<Device> device, const BufferDesc& desc)
    : mDesc(desc), mDevice(std::move(device)) {

    if (mDevice) {
        mWgpuDevice = *static_cast<wgpu::Device*>(mDevice->GetNativeDeviceHandle());
        mQueue      = *static_cast<wgpu::Queue*>(mDevice->GetNativeQueueHandle());
        mInstance   = *static_cast<wgpu::Instance*>(mDevice->GetNativeInstanceHandle());

        if (!mWgpuDevice) log::Error("BufferImpl: native device handle is null");
        if (!mQueue)      log::Error("BufferImpl: native queue handle is null");
        if (!mInstance)   log::Error("BufferImpl: native instance handle is null");
    }
}

result<void> BufferImpl::Initialize() noexcept {
    if (!mDevice)     return err(ErrorCode::INVALID_ARGUMENT, "BufferImpl::Initialize: device is null");
    if (!mWgpuDevice) return err(ErrorCode::INVALID_STATE,    "BufferImpl::Initialize: native device is null");
    if (!mQueue)      return err(ErrorCode::INVALID_STATE,    "BufferImpl::Initialize: native queue is null");
    if (!mInstance)   return err(ErrorCode::INVALID_STATE,    "BufferImpl::Initialize: native instance is null");

    if (mDesc.size == 0) {
        return err(ErrorCode::INVALID_ARGUMENT, "BufferImpl::Initialize: size must be > 0");
    }

    // WebGPU is happiest when buffer sizes are aligned.
    mAllocatedSize = AlignUp(mDesc.size, 4);

    wgpu::BufferDescriptor bd{};
    bd.size = static_cast<size_t>(mAllocatedSize);
    bd.usage = detail::ToWGPU(mDesc.usage);
    bd.mappedAtCreation = false;

    mBuffer = mWgpuDevice.CreateBuffer(&bd);
    if (!mBuffer) {
        log::Error("BufferImpl::Initialize: wgpu::Device::CreateBuffer failed");
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "BufferImpl::Initialize: CreateBuffer failed");
    }

    return ok();
}

result<void> BufferImpl::Update(u64 byteOffset, const void* data, u64 numBytes) noexcept {
    if (!mBuffer || !mQueue) {
        return err(ErrorCode::INVALID_STATE, "BufferImpl::Update: not initialized");
    }
    if (!data) {
        return err(ErrorCode::INVALID_ARGUMENT, "BufferImpl::Update: data is null");
    }
    if (numBytes == 0) {
        return err(ErrorCode::INVALID_ARGUMENT, "BufferImpl::Update: numBytes is 0");
    }

    if (!HasFlag(mDesc.usage, BufferUsageFlags::CopyDst)) {
        return err(ErrorCode::INVALID_STATE, "BufferImpl::Update: buffer missing CopyDst usage");
    }

    // WebGPU requires offset and size to be multiples of 4 for WriteBuffer.
    if ((byteOffset % 4u) != 0u || (numBytes % 4u) != 0u) {
        return err(ErrorCode::INVALID_ARGUMENT,
            "BufferImpl::Update: byteOffset and numBytes must be multiples of 4");
    }

    // Range must fit inside allocated buffer.
    if (byteOffset + numBytes > mAllocatedSize) {
        return err(ErrorCode::OUT_OF_RANGE, "BufferImpl::Update: write range exceeds buffer size");
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
        return err(ErrorCode::INVALID_STATE, "BufferImpl::MapImpl: not initialized");
    }

    if (!HasFlag(mDesc.usage, requiredFlag)) {
        const bool isWrite = (mode == wgpu::MapMode::Write);
        return err(ErrorCode::INVALID_STATE,
            isWrite ? "BufferImpl::MapWrite: buffer missing MapWrite usage"
                    : "BufferImpl::MapRead:  buffer missing MapRead usage");
    }

    // Safe alignment rules for WebGPU mapping:
    // - offset: commonly required to be 8-byte aligned
    // - size: must be multiple of 4 bytes (and non-zero)
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

    // IMPORTANT: wait; do not use timeout=0 (that only polls once).
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
