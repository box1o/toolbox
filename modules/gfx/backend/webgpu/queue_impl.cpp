#include "queue_impl.hpp"
#include "command_buffer_impl.hpp"
#include "toolbox/gfx/api/device.hpp"

namespace ct::gfx::webgpu {

QueueImpl::QueueImpl(ref<Device> device, const QueueDesc& desc)
    : mDesc(desc), mDevice(std::move(device)) {}

result<void> QueueImpl::Initialize() noexcept { return ok(); }

result<void> QueueImpl::Submit(const std::vector<ref<CommandBuffer>>& cmdBuffers) noexcept {
    if (!mDevice) {
        return err(ErrorCode::INVALID_STATE, "Queue: device is null");
    }

    auto* queue = static_cast<wgpu::Queue*>(mDevice->GetNativeQueueHandle());
    if (!queue || !*queue) {
        return err(ErrorCode::INVALID_STATE, "Queue: failed to acquire queue handle");
    }

    if (cmdBuffers.empty()) {
        queue->Submit(0, nullptr);
        return ok();
    }

    std::vector<wgpu::CommandBuffer> native;
    native.reserve(cmdBuffers.size());

    for (const auto& cmd : cmdBuffers) {
        if (!cmd) {
            return err(ErrorCode::INVALID_ARGUMENT, "Queue: command buffer is null");
        }

        auto* nativeCmd = static_cast<wgpu::CommandBuffer*>(cmd->GetNativeHandle());
        if (!nativeCmd || !*nativeCmd) {
            return err(ErrorCode::INVALID_STATE, "Queue: command buffer handle is null");
        }

        native.push_back(*nativeCmd);
    }

    queue->Submit(static_cast<uint32_t>(native.size()), native.data());
    return ok();
}

} // namespace ct::gfx::webgpu
