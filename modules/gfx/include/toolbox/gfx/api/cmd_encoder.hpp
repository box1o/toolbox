#pragma once
#include "toolbox/base/base.hpp"
#include "toolbox/gfx/common.hpp"
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx {



struct CommandEncoderDesc {
};


class CommandEncoder {
public:
    virtual ~CommandEncoder() = default;

    // [[nodiscard]] static result<ref<CommandBuffer>> Create(const CommandBufferDesc& desc) noexcept;
protected:
    CommandEncoder() = default;
};

} // namespace ct::gfx
