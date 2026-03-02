#pragma once
#include <toolbox/gfx/api/command_encoder.hpp>
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx::webgpu {

class CommandEncoderImpl final : public CommandEncoder {
public:
    explicit CommandEncoderImpl(ref<Device> device, const CommandEncoderDesc& desc);
    ~CommandEncoderImpl() override = default;

    [[nodiscard]] result<ref<RenderPassEncoder>> BeginRenderPass(
        const RenderPassDesc& desc) noexcept override;

    result<void> CopyBufferToBuffer(
        ref<Buffer> src, u64 srcOffset,
        ref<Buffer> dst, u64 dstOffset,
        u64 size) noexcept override;

    result<void> ClearBuffer(
        ref<Buffer> buffer, u64 offset, u64 size) noexcept override;

    result<void> CopyBufferToTexture(
        const BufferCopyView& src,
        const TextureCopyView& dst,
        const Extent3D& copySize) noexcept override;

    result<void> CopyTextureToBuffer(
        const TextureCopyView& src,
        const BufferCopyView& dst,
        const Extent3D& copySize) noexcept override;

    result<void> CopyTextureToTexture(
        const TextureCopyView& src,
        const TextureCopyView& dst,
        const Extent3D& copySize) noexcept override;

    [[nodiscard]] result<ref<CommandBuffer>> Finish(
        const CommandBufferDesc& desc) noexcept override;

    result<void> Initialize() noexcept;

private:
    CommandEncoderDesc mDesc{};
    ref<Device> mDevice{nullptr};
    wgpu::CommandEncoder mEncoder{nullptr};
    bool mFinished{false};
};

} // namespace ct::gfx::webgpu
