#include "toolbox/gfx/api/pipeline.hpp"

#if defined(USE_WEBGPU_BACKEND)
#include "../../backend/webgpu/pipeline_impl.hpp"
#endif

namespace ct::gfx {

result<ref<Pipeline>> Pipeline::Builder::Build() noexcept {
#if defined(USE_WEBGPU_BACKEND)
    auto pipeline = createRef<webgpu::PipelineImpl>(mDevice, mDesc);
    pipeline->SetShader(mShader);
    pipeline->SetVertexLayout(mVertexLayout);
    pipeline->SetBindGroupLayouts(mBindGroupLayouts);
    pipeline->SetRasterizerState(mRasterizer);
    pipeline->SetDepthStencilState(mDepthStencil);
    pipeline->SetColorTargetStates(mColorTargets);
    TRY_RETURN(pipeline->Initialize());
    return ok(std::move(pipeline));
#else
    log::Critical("Pipeline creation failed: no graphics backend available");
    std::abort();
#endif
}

// clang-format off
Pipeline::Builder& Pipeline::Builder::SetShader(ref<Shader> shader)                     { mShader = std::move(shader); return *this; }
Pipeline::Builder& Pipeline::Builder::SetVertexLayout(const VertexLayout& layout)       { mVertexLayout = layout; return *this; }
Pipeline::Builder& Pipeline::Builder::AddBindGroupLayout(const BindGroupLayout& layout) { mBindGroupLayouts.push_back(layout); return *this; }
Pipeline::Builder& Pipeline::Builder::SetCullMode(CullMode mode)                        { mRasterizer.cullMode = mode; return *this; }
Pipeline::Builder& Pipeline::Builder::SetFrontFace(FrontFace face)                      { mRasterizer.frontFace = face; return *this; }
Pipeline::Builder& Pipeline::Builder::SetTopology(PrimitiveTopology topology)           { mRasterizer.topology = topology; return *this; }
Pipeline::Builder& Pipeline::Builder::SetPolygonMode(PolygonMode mode)                  { mRasterizer.polygonMode = mode; return *this; }
Pipeline::Builder& Pipeline::Builder::SetDepthTest(bool enable)                         { mDepthStencil.depthTestEnable = enable; return *this; }
Pipeline::Builder& Pipeline::Builder::SetDepthWrite(bool enable)                        { mDepthStencil.depthWriteEnable = enable; return *this; }
Pipeline::Builder& Pipeline::Builder::SetDepthCompare(CompareOp op)                     { mDepthStencil.depthCompareOp = op; return *this; }
Pipeline::Builder& Pipeline::Builder::SetDepthFormat(TextureFormat format)              { mDepthStencil.depthFormat = format; return *this; }
Pipeline::Builder& Pipeline::Builder::SetStencilTest(bool enable)                       { mDepthStencil.stencilEnable = enable; return *this; }
Pipeline::Builder& Pipeline::Builder::SetBlendEnabled(bool enable)                      { if (mColorTargets.empty()) mColorTargets.push_back({}); mColorTargets.back().blend.enable = enable; return *this; }
Pipeline::Builder& Pipeline::Builder::SetBlendSrcColor(BlendFactor factor)              { if (mColorTargets.empty()) mColorTargets.push_back({}); mColorTargets.back().blend.srcColor = factor; return *this; }
Pipeline::Builder& Pipeline::Builder::SetBlendDstColor(BlendFactor factor)              { if (mColorTargets.empty()) mColorTargets.push_back({}); mColorTargets.back().blend.dstColor = factor; return *this; }
Pipeline::Builder& Pipeline::Builder::SetBlendColorOp(BlendOp op)                       { if (mColorTargets.empty()) mColorTargets.push_back({}); mColorTargets.back().blend.colorOp = op; return *this; }
Pipeline::Builder& Pipeline::Builder::SetBlendSrcAlpha(BlendFactor factor)              { if (mColorTargets.empty()) mColorTargets.push_back({}); mColorTargets.back().blend.srcAlpha = factor; return *this; }
Pipeline::Builder& Pipeline::Builder::SetBlendDstAlpha(BlendFactor factor)              { if (mColorTargets.empty()) mColorTargets.push_back({}); mColorTargets.back().blend.dstAlpha = factor; return *this; }
Pipeline::Builder& Pipeline::Builder::SetBlendAlphaOp(BlendOp op)                       { if (mColorTargets.empty()) mColorTargets.push_back({}); mColorTargets.back().blend.alphaOp = op; return *this; }
// clang-format on

Pipeline::Builder& Pipeline::Builder::SetColorTarget(
    TextureFormat format, const BlendState& blend) {
    mColorTargets.clear();
    ColorTargetState target{};
    target.format = format;
    target.blend = blend;
    mColorTargets.push_back(target);
    return *this;
}

Pipeline::Builder& Pipeline::Builder::AddColorTarget(
    TextureFormat format, const BlendState& blend) {
    ColorTargetState target{};
    target.format = format;
    target.blend = blend;
    mColorTargets.push_back(target);
    return *this;
}

} // namespace ct::gfx
