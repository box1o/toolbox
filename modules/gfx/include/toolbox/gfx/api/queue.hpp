#pragma once
#include <toolbox/base/base.hpp>
#include <toolbox/gfx/api/types.hpp>

namespace ct::gfx {

class Device;
class CommandBuffer;

struct QueueDesc {
    QueueType type{QueueType::Graphics};
    std::string debugName{"Queue"};
};

class Queue {
public:
    virtual ~Queue() = default;

    virtual result<void> Submit(std::initializer_list<ref<CommandBuffer>> cmdBuffers) noexcept = 0;

    [[nodiscard]] static result<ref<Queue>> Create(ref<Device> device, const QueueDesc& desc = {}) noexcept;

protected:
    Queue() = default;
};

} // namespace ct::gfx
