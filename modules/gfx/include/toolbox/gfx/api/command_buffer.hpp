#pragma once
#include <toolbox/base/base.hpp>

namespace ct::gfx {

class CommandBuffer {
public:
    virtual ~CommandBuffer() = default;
protected:
    CommandBuffer() = default;
};

} // namespace ct::gfx
