#include "queue_impl.hpp"
#include "command_impl.hpp"
#include "device_impl.hpp"

#include <toolbox/base/logger/logger.hpp>

namespace ct::gfx::webgpu {

result<void> QueueImpl::Initialize(ref<Device> device, const QueueDesc&) noexcept {
    if (!device) return err(ErrorCode::INVALID_ARGUMENT, "Queue: device null");

    auto* dev = dynamic_cast<DeviceImpl*>(device.get());
    if (!dev) {
        log::Error("QueueImpl: device is not WebGPU device");
        return err(ErrorCode::INVALID_ARGUMENT, "Queue: device is not WebGPU device");
    }

    mQueue = dev->QueueHandle();
    if (!mQueue) {
        log::Error("QueueImpl: failed to get device queue");
        return err(
            ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Queue: failed to get device queue");
    }

    return ok();
}

result<void> QueueImpl::Submit(std::initializer_list<ref<CommandBuffer>> cmdBuffers) noexcept {
    if (!mQueue) return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Queue: null");

    std::vector<wgpu::CommandBuffer> native;
    native.reserve(cmdBuffers.size());

    for (auto& cb : cmdBuffers) {
        auto* impl = dynamic_cast<CommandBufferImpl*>(cb.get());
        if (!impl)
            return err(ErrorCode::INVALID_ARGUMENT, "Queue: command buffer backend mismatch");
        native.push_back(impl->Handle());
    }

    if (!native.empty()) {
        mQueue.Submit(static_cast<uint32_t>(native.size()), native.data());
    }

    return ok();
}

} // namespace ct::gfx::webgpu
