#pragma once
#include <toolbox/gfx/api/command_buffer.hpp>
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx::webgpu {

class CommandBufferImpl final : public CommandBuffer {
public:
    explicit CommandBufferImpl(wgpu::CommandBuffer cmdBuffer, const CommandBufferDesc& desc);
    ~CommandBufferImpl() override = default;

    [[nodiscard]] void* GetNativeHandle() noexcept override {
        return static_cast<void*>(&mCommandBuffer);
    }

    [[nodiscard]] const wgpu::CommandBuffer& Handle() const noexcept { return mCommandBuffer; }

private:
    CommandBufferDesc mDesc{};
    wgpu::CommandBuffer mCommandBuffer{nullptr};
};

} // namespace ct::gfx::webgpu
