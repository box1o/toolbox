#pragma once
#include <toolbox/base/base.hpp>

namespace ct::gfx {

struct QueueDesc {};

class CommandBuffer;

// fwd
class Device;
class Queue {
public:
    virtual ~Queue() = default;

    // virtual void Resize(u32 width, u32 height) noexcept = 0;
    // [[nodiscard]] virtual void* GetNativeSurfaceHandle() noexcept = 0;

    virtual result<void> Submit(std::initializer_list<ref<CommandBuffer>> cmdBuffers) noexcept = 0;

    [[nodiscard]] static result<ref<Queue>> Create(
        ref<Device> device, const QueueDesc& desc = {}) noexcept;

protected:
    virtual result<void> Initialize() noexcept = 0;
    Queue() = default;
};

} // namespace ct::gfx
