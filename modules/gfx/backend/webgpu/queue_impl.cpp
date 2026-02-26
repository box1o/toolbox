#include "queue_impl.hpp"
#include "toolbox/gfx/api/device.hpp"

namespace ct::gfx::webgpu {

// NOTE: DeviceImpl
QueueImpl::QueueImpl(ref<Device> device, const QueueDesc& desc) : mDesc(desc), mDevice(device) {}

result<void> QueueImpl::Initialize() noexcept { return ok(); }

result<void> QueueImpl::Submit(std::initializer_list<ref<CommandBuffer>> cmdBuffers) noexcept {
    if (!mDevice) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "Queue: device null");
    }

    auto queue = static_cast<wgpu::Queue*>(mDevice->GetNativeQueueHandle());
    if (!queue) {
        return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
            "Queue: Filed to aquire the device queue handle");
    }

    std::vector<wgpu::CommandBuffer> native;
    native.reserve(cmdBuffers.size());

    // for (const auto& cmd : cmdBuffers) {
    //     auto nativeCmd = static_cast<wgpu::CommandBuffer*>(cmd->GetNativeCommandBufferHandle());
    //     if (!nativeCmd) {
    //         return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED,
    //             "Queue: Failed to aquire command buffer handle");
    //     }
    //     native.push_back(*nativeCmd);
    // }

    // for (auto& cb : cmdBuffers) {
    //     auto* impl = dynamic_cast<CommandBufferImpl*>(cb.get());
    //     if (!impl)
    //         return err(ErrorCode::INVALID_ARGUMENT, "Queue: command buffer backend mismatch");
    //     native.push_back(impl->Handle());
    // }
    //
    // if (!native.empty()) {
    //     mQueue.Submit(static_cast<uint32_t>(native.size()), native.data());
    // }

    return ok();
};

} // namespace ct::gfx::webgpu
