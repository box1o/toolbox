#pragma once
#include "toolbox/base/base.hpp"
#include "toolbox/gfx/api/vertex_layout.hpp"
#include "toolbox/gfx/types.hpp"
#include <webgpu/webgpu_cpp.h>
#include <vector>

namespace ct {

class Device;
class Shader;
class BindGroupLayout;

enum class PrimitiveTopology : u8 {
    PointList,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip,
};

enum class CullMode : u8 { None, Front, Back };
enum class FrontFace : u8 { CCW, CW };

enum class BlendFactor : u8 {
    Zero,
    One,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha,
    SrcColor,
    OneMinusSrcColor,
    DstColor,
    OneMinusDstColor,
};

enum class BlendOp : u8 { Add, Subtract, ReverseSubtract, Min, Max };

struct BlendState {
    bool enabled{false};
    BlendFactor srcColor{BlendFactor::SrcAlpha};
    BlendFactor dstColor{BlendFactor::OneMinusSrcAlpha};
    BlendOp colorOp{BlendOp::Add};
    BlendFactor srcAlpha{BlendFactor::One};
    BlendFactor dstAlpha{BlendFactor::Zero};
    BlendOp alphaOp{BlendOp::Add};

    [[nodiscard]] static constexpr BlendState Opaque() noexcept { return {}; }

    [[nodiscard]] static constexpr BlendState AlphaBlend() noexcept {
        BlendState s{};
        s.enabled = true;
        return s;
    }

    [[nodiscard]] static constexpr BlendState Additive() noexcept {
        BlendState s{};
        s.enabled = true;
        s.srcColor = BlendFactor::One;
        s.dstColor = BlendFactor::One;
        s.srcAlpha = BlendFactor::One;
        s.dstAlpha = BlendFactor::One;
        return s;
    }
};

struct DepthStencilState {
    bool depthTestEnabled{true};
    bool depthWriteEnabled{true};
    CompareFunction depthCompare{CompareFunction::Less};
    TextureFormat format{TextureFormat::Depth24PlusStencil8};

    [[nodiscard]] static constexpr DepthStencilState Default() noexcept { return {}; }

    [[nodiscard]] static constexpr DepthStencilState ReadOnly() noexcept {
        DepthStencilState s{};
        s.depthWriteEnabled = false;
        return s;
    }

    [[nodiscard]] static constexpr DepthStencilState Disabled() noexcept {
        DepthStencilState s{};
        s.depthTestEnabled = false;
        s.depthWriteEnabled = false;
        return s;
    }
};

struct RenderPipelineInfo {
    ref<Shader> shader;
    VertexLayout vertexLayout{};
    std::vector<ref<BindGroupLayout>> bindGroupLayouts{};
    std::vector<TextureFormat> colorFormats{};
    BlendState blend{};
    DepthStencilState depthStencil{};
    PrimitiveTopology topology{PrimitiveTopology::TriangleList};
    CullMode cullMode{CullMode::Back};
    FrontFace frontFace{FrontFace::CCW};
    bool hasDepth{true};
    u32 sampleCount{1};

    RenderPipelineInfo& SetShader(ref<Shader> s) {
        shader = std::move(s);
        return *this;
    }

    RenderPipelineInfo& SetVertexLayout(const VertexLayout& layout) {
        vertexLayout = layout;
        return *this;
    }

    RenderPipelineInfo& AddBindGroupLayout(ref<BindGroupLayout> layout) {
        bindGroupLayouts.push_back(std::move(layout));
        return *this;
    }

    RenderPipelineInfo& AddColorFormat(TextureFormat format) {
        colorFormats.push_back(format);
        return *this;
    }

    RenderPipelineInfo& SetBlend(const BlendState& b) {
        blend = b;
        return *this;
    }

    RenderPipelineInfo& SetDepthStencil(const DepthStencilState& ds) {
        depthStencil = ds;
        return *this;
    }

    RenderPipelineInfo& SetTopology(PrimitiveTopology t) {
        topology = t;
        return *this;
    }

    RenderPipelineInfo& SetCullMode(CullMode m) {
        cullMode = m;
        return *this;
    }

    RenderPipelineInfo& SetFrontFace(FrontFace f) {
        frontFace = f;
        return *this;
    }

    RenderPipelineInfo& SetDepthEnabled(bool enabled) {
        hasDepth = enabled;
        return *this;
    }

    RenderPipelineInfo& SetSampleCount(u32 count) {
        sampleCount = count;
        return *this;
    }
};

class RenderPipeline {
public:
    ~RenderPipeline();

    [[nodiscard]] wgpu::RenderPipeline GetHandle() const noexcept;

    [[nodiscard]] static result<ref<RenderPipeline>> Create(
        ref<Device> device, const RenderPipelineInfo& info) noexcept;

private:
    RenderPipeline() = default;

    wgpu::RenderPipeline mPipeline{nullptr};
};

} // namespace ct
