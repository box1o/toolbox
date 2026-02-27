#pragma once
#include <string>
#include <toolbox/base/base.hpp>
#include <toolbox/gfx/api/vertex_layout.hpp>
#include <toolbox/gfx/api/bind_group_layout.hpp>
#include <toolbox/gfx/api/texture.hpp>

namespace ct::gfx {

enum class FrontFace : u8 { CCW, CW };
enum class CullMode : u8 { None, Front, Back };
enum class CompareOp : u8 { Never, Less, Equal, LessEqual, Greater, NotEqual, GreaterEqual, Always };
enum class PrimitiveTopology : u8 { TriangleList, TriangleStrip, LineList, LineStrip, PointList };

enum class BlendFactor : u8 {
    Zero, One,
    SrcColor, OneMinusSrcColor,
    DstColor, OneMinusDstColor,
    SrcAlpha, OneMinusSrcAlpha,
    DstAlpha, OneMinusDstAlpha,
};

enum class BlendOp : u8 { Add, Subtract, ReverseSubtract, Min, Max };
enum class PolygonMode : u8 { Fill, Line, Point };

struct RasterizerState {
    PolygonMode polygonMode{PolygonMode::Fill};
    CullMode cullMode{CullMode::Back};
    FrontFace frontFace{FrontFace::CCW};
    PrimitiveTopology topology{PrimitiveTopology::TriangleList};
};

struct DepthStencilState {
    bool depthTestEnable{true};
    bool depthWriteEnable{true};
    CompareOp depthCompareOp{CompareOp::Less};
    bool stencilEnable{false};
    TextureFormat depthFormat{TextureFormat::Depth24PlusStencil8};
};

struct BlendState {
    bool enable{false};
    BlendFactor srcColor{BlendFactor::One};
    BlendFactor dstColor{BlendFactor::Zero};
    BlendOp colorOp{BlendOp::Add};
    BlendFactor srcAlpha{BlendFactor::One};
    BlendFactor dstAlpha{BlendFactor::Zero};
    BlendOp alphaOp{BlendOp::Add};
};

struct ColorTargetState {
    TextureFormat format{TextureFormat::BGRA8Unorm};
    BlendState blend{};
};

struct PipelineDesc {
    std::string label{"pipeline"};
};

class Device;
class Shader;

class Pipeline {
public:
    virtual ~Pipeline() = default;

    [[nodiscard]] virtual void* GetNativePipelineHandle() noexcept = 0;

    class Builder {
    public:
        Builder(ref<Device> dev, const PipelineDesc& d)
            : mDevice(std::move(dev)), mDesc(d) {}

        // clang-format off
        Builder& SetShader(ref<Shader> shader);
        Builder& SetVertexLayout(const VertexLayout& layout);
        Builder& AddBindGroupLayout(const BindGroupLayout& layout);
        Builder& SetCullMode(CullMode mode);
        Builder& SetFrontFace(FrontFace face);
        Builder& SetTopology(PrimitiveTopology topology);
        Builder& SetPolygonMode(PolygonMode mode);
        Builder& SetDepthTest(bool enable);
        Builder& SetDepthWrite(bool enable);
        Builder& SetDepthCompare(CompareOp op);
        Builder& SetDepthFormat(TextureFormat format);
        Builder& SetStencilTest(bool enable);
        Builder& SetColorTarget(TextureFormat format, const BlendState& blend = {});
        Builder& SetBlendEnabled(bool enable);
        Builder& SetBlendSrcColor(BlendFactor factor);
        Builder& SetBlendDstColor(BlendFactor factor);
        Builder& SetBlendColorOp(BlendOp op);
        Builder& SetBlendSrcAlpha(BlendFactor factor);
        Builder& SetBlendDstAlpha(BlendFactor factor);
        Builder& SetBlendAlphaOp(BlendOp op);
        // clang-format on

        result<ref<Pipeline>> Build() noexcept;

    private:
        friend class Pipeline;

        ref<Device> mDevice{};
        PipelineDesc mDesc{};
        ref<Shader> mShader{};

        VertexLayout mVertexLayout{};
        std::vector<BindGroupLayout> mBindGroupLayouts{};

        RasterizerState mRasterizer{};
        DepthStencilState mDepthStencil{};
        ColorTargetState mColorTarget{};
    };

    [[nodiscard]] static Builder Create(ref<Device> device, const PipelineDesc& desc = {}) noexcept {
        return Builder{std::move(device), desc};
    }

protected:
    virtual result<void> Initialize() noexcept = 0;
    Pipeline() = default;
};

} // namespace ct::gfx
