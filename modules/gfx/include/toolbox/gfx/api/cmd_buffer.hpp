
#pragma once
#include "toolbox/base/base.hpp"
#include "toolbox/gfx/common.hpp"
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx {



struct CommandBufferDesc {
};


class CommandBuffer {
public:
    virtual ~CommandBuffer() = default;

    // [[nodiscard]] static result<ref<CommandBuffer>> Create(const CommandBufferDesc& desc) noexcept;
protected:
    CommandBuffer() = default;
};

} // namespace ct::gfx
