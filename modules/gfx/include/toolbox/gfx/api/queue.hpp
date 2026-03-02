#pragma once
#include <toolbox/base/base.hpp>
#include <vector>

namespace ct::gfx {

struct QueueDesc {};

class Device;
class CommandBuffer;

class Queue {
public:
    virtual ~Queue() = default;

    virtual result<void> Submit(const std::vector<ref<CommandBuffer>>& cmdBuffers) noexcept = 0;

    [[nodiscard]] static result<ref<Queue>> Create(
        ref<Device> device, const QueueDesc& desc = {}) noexcept;

protected:
    virtual result<void> Initialize() noexcept = 0;
    Queue() = default;
};

} // namespace ct::gfx
