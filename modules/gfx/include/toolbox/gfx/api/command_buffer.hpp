#pragma once
#include <toolbox/base/base.hpp>
#include <string>

namespace ct::gfx {

struct CommandBufferDesc {
    std::string debugName{"CommandBuffer"};
};

class CommandBuffer {
public:
    virtual ~CommandBuffer() = default;

    [[nodiscard]] virtual void* GetNativeHandle() noexcept = 0;

protected:
    CommandBuffer() = default;
};

} // namespace ct::gfx
