#pragma once
#include <toolbox/base/base.hpp>
#include <toolbox/gfx/api/render_pass.hpp>

namespace ct::gfx {

class Device;
class CommandBuffer;
class RenderPassEncoder;

struct CommandEncoderDesc {
    std::string debugName{"CommandEncoder"};
};

class CommandEncoder {
public:
    virtual ~CommandEncoder() = default;

    [[nodiscard]] virtual result<ref<RenderPassEncoder>> BeginRenderPass(const RenderPassDesc& desc) noexcept = 0;
    [[nodiscard]] virtual result<ref<CommandBuffer>> Finish() noexcept = 0;

    [[nodiscard]] static result<ref<CommandEncoder>> Create(ref<Device> device, const CommandEncoderDesc& desc = {}) noexcept;

protected:
    CommandEncoder() = default;
};

} // namespace ct::gfx
