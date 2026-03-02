#include "command_buffer_impl.hpp"

namespace ct::gfx::webgpu {

CommandBufferImpl::CommandBufferImpl(wgpu::CommandBuffer cmdBuffer, const CommandBufferDesc& desc)
    : mDesc(desc), mCommandBuffer(std::move(cmdBuffer)) {}

} // namespace ct::gfx::webgpu
