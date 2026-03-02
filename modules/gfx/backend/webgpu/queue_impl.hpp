#pragma once
#include <toolbox/gfx/api/queue.hpp>
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx::webgpu {

class QueueImpl final : public Queue {
public:
    explicit QueueImpl(ref<Device> device, const QueueDesc& desc);
    ~QueueImpl() override = default;

    result<void> Submit(const std::vector<ref<CommandBuffer>>& cmdBuffers) noexcept override;

    result<void> Initialize() noexcept override;

private:
    QueueDesc mDesc{};
    ref<Device> mDevice{nullptr};
};

} // namespace ct::gfx::webgpu
