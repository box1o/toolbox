#pragma once
#include <toolbox/gfx/api/queue.hpp>
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx::webgpu {

class QueueImpl final : public Queue {
public:
    explicit QueueImpl(ref<Device> device, const QueueDesc& desc);
    ~QueueImpl() override = default;

    result<void> Submit(std::initializer_list<ref<CommandBuffer>> cmdBuffers) noexcept override;

    // NOTE: Initialize
    result<void> Initialize() noexcept override;

private:
private:
    QueueDesc mDesc{};

    ref<Device> mDevice{nullptr};
    // wgpu::Queue mQueue;
};

} // namespace ct::gfx::webgpu
