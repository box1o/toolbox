#pragma once
#include <toolbox/gfx/api/queue.hpp>

#include <webgpu/webgpu_cpp.h>

namespace ct::gfx {
class Device;
}

namespace ct::gfx::webgpu {

class QueueImpl final : public Queue {
public:
    QueueImpl() = default;
    ~QueueImpl() override = default;

    bool Init(ref<Device> device, const QueueDesc& desc) noexcept;

    result<void> Submit(std::initializer_list<ref<CommandBuffer>> cmdBuffers) noexcept override;

private:
    wgpu::Queue mQueue;
};

} // namespace ct::gfx::webgpu
