#pragma once
#include <toolbox/gfx/api/command_encoder.hpp>
#include <toolbox/gfx/api/command_buffer.hpp>
#include <toolbox/gfx/api/render_pass.hpp>

#include <webgpu/webgpu_cpp.h>
#include <vector>

namespace ct::gfx {
class Device;
}

namespace ct::gfx::webgpu {

class RenderPassEncoderImpl final : public RenderPassEncoder {
public:
    explicit RenderPassEncoderImpl(wgpu::RenderPassEncoder pass) : mPass(pass) {}
    ~RenderPassEncoderImpl() override = default;

    void BindPipeline(ref<RenderPipeline> pipeline) noexcept override;
    void BindBindGroup(u32 setIndex, ref<BindGroup> group) noexcept override;

    void BindVertexBuffer(u32 slot, ref<Buffer> buffer, u64 offset = 0) noexcept override;
    void BindIndexBuffer(ref<Buffer> buffer, IndexFormat fmt, u64 offset = 0) noexcept override;

    void Draw(const DrawDesc& desc) noexcept override;
    void DrawIndexed(const DrawIndexedDesc& desc) noexcept override;

    void End() noexcept override {
        if (mPass) {
            mPass.End();
            mPass = nullptr;
        }
    }

private:
    wgpu::RenderPassEncoder mPass;
};

class CommandBufferImpl final : public CommandBuffer {
public:
    explicit CommandBufferImpl(wgpu::CommandBuffer buf) : mBuf(buf) {}
    ~CommandBufferImpl() override = default;

    [[nodiscard]] const wgpu::CommandBuffer& Handle() const noexcept { return mBuf; }

private:
    wgpu::CommandBuffer mBuf;
};

class CommandEncoderImpl final : public CommandEncoder {
public:
    CommandEncoderImpl() = default;
    ~CommandEncoderImpl() override = default;

    bool Init(ref<Device> device, const CommandEncoderDesc& desc) noexcept;

    result<ref<RenderPassEncoder>> BeginRenderPass(const RenderPassDesc& desc) noexcept override;
    result<ref<CommandBuffer>> Finish() noexcept override;

private:
    wgpu::Device mDevice;
    wgpu::CommandEncoder mEncoder;
    bool mFinished{false};

    // Keep all view wrappers alive until Finish()
    std::vector<wgpu::TextureView> mKeepAliveViews;

    // Storage for attachments during BeginRenderPass()
    std::vector<wgpu::RenderPassColorAttachment> mColorAttachments;
};

} // namespace ct::gfx::webgpu
