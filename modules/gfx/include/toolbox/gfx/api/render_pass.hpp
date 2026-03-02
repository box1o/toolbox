#pragma once
#include <toolbox/base/base.hpp>
#include <toolbox/gfx/api/texture.hpp>
#include <vector>
#include <string>

namespace ct::gfx {

class Buffer;
class Pipeline;
class BindGroup;

enum class IndexFormat : u8 {
    Uint16,
    Uint32,
};

struct ClearColor {
    f32 r{0.0f}, g{0.0f}, b{0.0f}, a{1.0f};
};

struct ColorAttachmentDesc {
    void* view{nullptr};
    ClearColor clear{0.1f, 0.1f, 0.12f, 1.0f};
    bool clearEnabled{true};
};

struct RenderPassDesc {
    std::vector<ColorAttachmentDesc> colors{};
    bool enableDepth{false};
    void* depthView{nullptr};
    f32 clearDepth{1.0f};
    u32 clearStencil{0};
    std::string debugName{"RenderPass"};
};

struct DrawDesc {
    u32 vertexCount{0};
    u32 instanceCount{1};
    u32 firstVertex{0};
    u32 firstInstance{0};
};

struct DrawIndexedDesc {
    u32 indexCount{0};
    u32 instanceCount{1};
    u32 firstIndex{0};
    i32 vertexOffset{0};
    u32 firstInstance{0};
};

struct ViewportDesc {
    f32 x{0.0f};
    f32 y{0.0f};
    f32 width{0.0f};
    f32 height{0.0f};
    f32 minDepth{0.0f};
    f32 maxDepth{1.0f};
};

struct ScissorDesc {
    u32 x{0};
    u32 y{0};
    u32 width{0};
    u32 height{0};
};

class RenderPassEncoder {
public:
    virtual ~RenderPassEncoder() = default;

    virtual void SetPipeline(ref<Pipeline> pipeline) noexcept = 0;
    virtual void SetBindGroup(u32 groupIndex, ref<BindGroup> group, u32 dynamicOffsetCount = 0, const u32* dynamicOffsets = nullptr) noexcept = 0;
    virtual void SetVertexBuffer(u32 slot, ref<Buffer> buffer, u64 offset = 0, u64 size = 0) noexcept = 0;
    virtual void SetIndexBuffer(ref<Buffer> buffer, IndexFormat fmt, u64 offset = 0, u64 size = 0) noexcept = 0;

    virtual void SetViewport(const ViewportDesc& viewport) noexcept = 0;
    virtual void SetScissorRect(const ScissorDesc& scissor) noexcept = 0;
    virtual void SetBlendConstant(f32 r, f32 g, f32 b, f32 a) noexcept = 0;
    virtual void SetStencilReference(u32 reference) noexcept = 0;

    virtual void Draw(const DrawDesc& desc) noexcept = 0;
    virtual void DrawIndexed(const DrawIndexedDesc& desc) noexcept = 0;

    virtual void End() noexcept = 0;

protected:
    RenderPassEncoder() = default;
};

} // namespace ct::gfx
