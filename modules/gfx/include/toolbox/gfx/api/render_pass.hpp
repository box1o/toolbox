#pragma once
#include <toolbox/base/base.hpp>
#include <toolbox/gfx/api/texture.hpp>

#include <vector>

namespace ct::gfx {

class Buffer;
class RenderPipeline;
class BindGroup;

struct ClearColor {
    float r{0}, g{0}, b{0}, a{1};
};

struct ColorAttachmentDesc {
    void* view{nullptr};            // expects backend C++ wrapper ptr (wgpu::TextureView*)
    ClearColor clear{0.1f, 0.1f, 0.12f, 1.0f};
    bool clearEnabled{true};
};

struct RenderPassDesc {
    std::vector<ColorAttachmentDesc> colors{};

    bool enableDepth{false};
    void* depthView{nullptr};       // expects backend C++ wrapper ptr
    float clearDepth{1.0f};
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

class RenderPassEncoder {
public:
    virtual ~RenderPassEncoder() = default;

    virtual void BindPipeline(ref<RenderPipeline> pipeline) noexcept = 0;
    virtual void BindBindGroup(u32 setIndex, ref<BindGroup> group) noexcept = 0;

    virtual void BindVertexBuffer(u32 slot, ref<Buffer> buffer, u64 offset = 0) noexcept = 0;
    virtual void BindIndexBuffer(ref<Buffer> buffer, IndexFormat fmt, u64 offset = 0) noexcept = 0;

    virtual void Draw(const DrawDesc& desc) noexcept = 0;
    virtual void DrawIndexed(const DrawIndexedDesc& desc) noexcept = 0;

    virtual void End() noexcept = 0;

protected:
    RenderPassEncoder() = default;
};

} // namespace ct::gfx
