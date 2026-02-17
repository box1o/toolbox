#pragma once
#include "toolbox/base/base.hpp"
#include "toolbox/gfx/api/buffer.hpp"
#include "toolbox/gfx/api/surface.hpp"
#include "toolbox/gfx/types.hpp"
#include <webgpu/webgpu_cpp.h>

namespace ct {

class Device;
class RenderPipeline;
class BindGroup;
class Surface;

struct ClearColor {
    f64 r{0.0};
    f64 g{0.0};
    f64 b{0.0};
    f64 a{1.0};
};

struct RenderPassInfo {
    wgpu::TextureView colorView{nullptr};
    wgpu::TextureView depthView{nullptr};
    ClearColor clearColor{};
    f32 clearDepth{1.0f};
    u32 clearStencil{0};
    bool loadColor{false};
    bool loadDepth{false};
};

enum class IndexFormat : u8 { U16, U32 };

class RenderPass {
public:
    ~RenderPass();

    RenderPass(const RenderPass&) = delete;
    RenderPass& operator=(const RenderPass&) = delete;
    RenderPass(RenderPass&&) noexcept;
    RenderPass& operator=(RenderPass&&) noexcept;

    void SetPipeline(const RenderPipeline& pipeline);
    void SetBindGroup(u32 index, const BindGroup& group);

    void SetVertexBuffer(u32 slot, const Buffer& buffer, u64 offset = 0);
    void SetIndexBuffer(const Buffer& buffer, IndexFormat format = IndexFormat::U16, u64 offset = 0);

    void Draw(u32 vertexCount, u32 instanceCount = 1, u32 firstVertex = 0, u32 firstInstance = 0);
    void DrawIndexed(u32 indexCount, u32 instanceCount = 1,
        u32 firstIndex = 0, i32 baseVertex = 0, u32 firstInstance = 0);

    void SetViewport(f32 x, f32 y, f32 width, f32 height, f32 minDepth = 0.0f, f32 maxDepth = 1.0f);
    void SetScissor(u32 x, u32 y, u32 width, u32 height);

    //NOTE: ends the pass, submits commands to the queue
    void Submit();

    [[nodiscard]] static result<RenderPass> Begin(
        Device& device, const RenderPassInfo& info) noexcept;

    //NOTE: convenience — creates RenderPassInfo from the current Surface frame
    [[nodiscard]] static result<RenderPass> Begin(
        Device& device, const Frame& frame, const ClearColor& clear = {}) noexcept;

private:
    RenderPass() = default;

    wgpu::CommandEncoder mEncoder{nullptr};
    wgpu::RenderPassEncoder mPass{nullptr};
    Device* mDevice{nullptr};
    bool mSubmitted{false};
};

} // namespace ct
