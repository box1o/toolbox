#pragma once
#include <toolbox/gfx/api/pipeline.hpp>
#include <toolbox/gfx/api/vertex_layout.hpp>
#include <toolbox/gfx/api/bind_group_layout.hpp>
#include <webgpu/webgpu_cpp.h>

namespace ct::gfx::webgpu {

class PipelineImpl final : public Pipeline {
public:
    explicit PipelineImpl(ref<Device> device, const PipelineDesc& desc);
    ~PipelineImpl() override = default;

    [[nodiscard]] void* GetNativePipelineHandle() noexcept override {
        return static_cast<void*>(&mPipeline);
    }

    void SetShader(ref<Shader> shader) { mShader = std::move(shader); }
    void SetVertexLayout(const VertexLayout& layout) { mVertexLayout = layout; }
    void SetBindGroupLayouts(const std::vector<BindGroupLayout>& layouts) { mBindGroupLayouts = layouts; }
    void SetRasterizerState(const RasterizerState& state) { mRasterizer = state; }
    void SetDepthStencilState(const DepthStencilState& state) { mDepthStencil = state; }
    void SetColorTargetState(const ColorTargetState& state) { mColorTarget = state; }

    result<void> Initialize() noexcept override;

private:
    result<wgpu::PipelineLayout> CreatePipelineLayout(wgpu::Device& device) noexcept;

    PipelineDesc mDesc{};
    ref<Device> mDevice{nullptr};
    ref<Shader> mShader{nullptr};

    VertexLayout mVertexLayout{};
    std::vector<BindGroupLayout> mBindGroupLayouts{};

    RasterizerState mRasterizer{};
    DepthStencilState mDepthStencil{};
    ColorTargetState mColorTarget{};

    wgpu::RenderPipeline mPipeline{nullptr};
};

} // namespace ct::gfx::webgpu
