#pragma once
#include "toolbox/base/base.hpp"
#include "toolbox/gfx/common.hpp"
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx {



struct QueueDesc {
};


class Device;
class Queue {
public:
    virtual ~Queue() = default;

    [[nodiscard]] static result<ref<Queue>> Create(ref<Device> device , const QueueDesc& desc) noexcept;
protected:
    Queue() = default;
};

} // namespace ct::gfx
