#pragma once
#include <toolbox/base/base.hpp>
#include <toolbox/gfx/api/texture.hpp>
#include <toolbox/gfx/api/vertex_layout.hpp>

namespace ct::gfx {

class Device;
class ShaderModule;
class PipelineLayout;

enum class PrimitiveTopology : u8 {
    TriangleList,
    TriangleStrip,
    LineList,
    LineStrip,
    PointList,
};

enum class CullMode : u8 { None, Front, Back };
enum class FrontFace : u8 { CCW, CW };

struct RasterState {
    PrimitiveTopology topology{PrimitiveTopology::TriangleList};
    CullMode cull{CullMode::Back};
    FrontFace frontFace{FrontFace::CCW};
};

struct RenderPipelineDesc {
    ref<ShaderModule> vertexShader{};
    ref<ShaderModule> fragmentShader{};

    VertexLayout vertexLayout{VertexLayoutDesc{}};

    // NEW: optional pipeline layout (bind group layouts)
    ref<PipelineLayout> layout{};

    TextureFormat colorFormat{TextureFormat::BGRA8Unorm};

    bool enableDepth{false};
    TextureFormat depthFormat{TextureFormat::Depth24PlusStencil8};

    RasterState raster{};

    std::string debugName{"RenderPipeline"};
};

class RenderPipeline {
public:
    virtual ~RenderPipeline() = default;

    [[nodiscard]] virtual void* GetNativePipeline() const noexcept = 0;

    [[nodiscard]] static result<ref<RenderPipeline>> Create(
        ref<Device> device, const RenderPipelineDesc& desc) noexcept;

protected:
    RenderPipeline() = default;
};

} // namespace ct::gfx
