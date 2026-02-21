#pragma once
#include <toolbox/base/base.hpp>
#include <toolbox/gfx/api/texture.hpp>
#include <toolbox/gfx/api/texture_resource.hpp>
#include <toolbox/gfx/api/render_pass.hpp>

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace ct::gfx {

class Device;
class Swapchain;
class CommandBuffer;

struct FGTextureHandle {
    u32 id{0};
    [[nodiscard]] explicit operator bool() const noexcept { return id != 0; }
};

class FrameGraph;

class PassBuilder {
public:
    PassBuilder(FrameGraph& fg, u32 passId);

    PassBuilder& Read(FGTextureHandle tex);

    PassBuilder& WriteColor(const std::string& name, TextureFormat fmt);
    PassBuilder& WriteDepth(const std::string& name, TextureFormat fmt);
    PassBuilder& WriteSwapchain();

    PassBuilder& ClearColor(u32 attachmentIndex, ClearColor c) noexcept;
    PassBuilder& ClearDepth(float depth = 1.0f, u32 stencil = 0) noexcept;

    PassBuilder& Execute(std::function<void(RenderPassEncoder& pass)> fn);

    [[nodiscard]] FGTextureHandle Color(const std::string& name) const;
    [[nodiscard]] FGTextureHandle Depth(const std::string& name) const;

private:
    FrameGraph& mFG;
    u32 mPassId{0};
};

class CompiledFrameGraph {
public:
    CompiledFrameGraph() = default;

    [[nodiscard]] ref<CommandBuffer> GetCommandBuffer() const noexcept { return mCmd; }

    [[nodiscard]] ref<TextureView> GetView(const std::string& name) const noexcept;
    [[nodiscard]] ref<Texture> GetTexture(const std::string& name) const noexcept;

    [[nodiscard]] ref<TextureView> GetView(FGTextureHandle h) const noexcept;
    [[nodiscard]] ref<Texture> GetTexture(FGTextureHandle h) const noexcept;

private:
    friend class FrameGraph;

    ref<CommandBuffer> mCmd{};
    std::unordered_map<std::string, FGTextureHandle> mNameToHandle;

    std::vector<ref<Texture>> mTextures;     // index = handle.id - 1
    std::vector<ref<TextureView>> mViews;    // index = handle.id - 1

    // which resources were transient and owned by FG for this compile
    std::vector<bool> mWasTransient;
};

class FrameGraph {
public:
    explicit FrameGraph(ref<Device> device);

    PassBuilder AddPass(const std::string& name);

    // Recommended
    [[nodiscard]] result<CompiledFrameGraph> Compile(ref<Swapchain> swapchain);

    // Convenience
    [[nodiscard]] result<ref<CommandBuffer>> CompileAndRecord(ref<Swapchain> swapchain);

    // Return transient resources to pool (call after you’re done submitting/presenting)
    void Recycle(const CompiledFrameGraph& compiled) noexcept;

    // External resources (history buffers, shadow maps, etc.)
    // These are never resized or pooled by FrameGraph.
    FGTextureHandle ImportTexture(
        const std::string& name,
        ref<Texture> texture,
        ref<TextureView> view,
        TextureFormat fmt,
        bool depth = false);

    void Reset();

private:
    friend class PassBuilder;

    struct TextureResource {
        std::string name{};
        TextureFormat format{TextureFormat::Undefined};
        bool isDepth{false};

        bool imported{false};
        bool transient{true}; // if false, keeps a dedicated allocation

        // realized/imported
        ref<Texture> texture{};
        ref<TextureView> view{};
        u32 width{0};
        u32 height{0};
    };

    struct Pass {
        std::string name{};

        std::vector<FGTextureHandle> reads{};
        std::vector<FGTextureHandle> writesColor{};
        FGTextureHandle writeDepth{};
        bool writesSwapchain{false};

        std::vector<ClearColor> clearColors{};
        bool hasDepthClear{false};
        float clearDepth{1.0f};
        u32 clearStencil{0};

        std::function<void(RenderPassEncoder&)> exec{};
    };

    struct PoolKey {
        TextureFormat fmt{TextureFormat::Undefined};
        bool depth{false};
        u32 w{0};
        u32 h{0};

        bool operator==(const PoolKey& o) const noexcept {
            return fmt == o.fmt && depth == o.depth && w == o.w && h == o.h;
        }
    };

    struct PoolKeyHash {
        size_t operator()(const PoolKey& k) const noexcept {
            size_t h = 1469598103934665603ull;
            auto mix = [&](u64 v) {
                h ^= (size_t)v;
                h *= 1099511628211ull;
            };
            mix((u64)k.fmt);
            mix((u64)k.depth);
            mix((u64)k.w);
            mix((u64)k.h);
            return h;
        }
    };

    struct PooledTex {
        ref<Texture> tex{};
        ref<TextureView> view{};
    };

private:
    FGTextureHandle GetOrCreateResource(const std::string& name, TextureFormat fmt, bool depth);
    TextureResource* GetResource(FGTextureHandle h);

    bool RealizeResource(TextureResource& r, u32 w, u32 h);

    PoolKey MakeKey(const TextureResource& r, u32 w, u32 h) const noexcept;
    PooledTex AcquireFromPool(const PoolKey& key) noexcept;
    void ReturnToPool(const PoolKey& key, PooledTex p) noexcept;

private:
    ref<Device> mDevice;

    std::vector<Pass> mPasses;

    std::vector<TextureResource> mResources; // index = handle.id - 1
    std::unordered_map<std::string, FGTextureHandle> mNameToHandle;

    std::unordered_map<PoolKey, std::vector<PooledTex>, PoolKeyHash> mPool;
};

} // namespace ct::gfx
