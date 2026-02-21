#include <toolbox/gfx/api/frame_graph.hpp>
#include <toolbox/gfx/api/render_pass.hpp> // ensure ClearColor is visible

#include <toolbox/base/logger/logger.hpp>
#include <toolbox/gfx/api/device.hpp>
#include <toolbox/gfx/api/swapchain.hpp>
#include <toolbox/gfx/api/command_encoder.hpp>
#include <toolbox/gfx/api/command_buffer.hpp>

namespace ct::gfx {

// ---------------- CompiledFrameGraph ----------------

ref<TextureView> CompiledFrameGraph::GetView(const std::string& name) const noexcept {
    auto it = mNameToHandle.find(name);
    if (it == mNameToHandle.end()) return {};
    return GetView(it->second);
}

ref<Texture> CompiledFrameGraph::GetTexture(const std::string& name) const noexcept {
    auto it = mNameToHandle.find(name);
    if (it == mNameToHandle.end()) return {};
    return GetTexture(it->second);
}

ref<TextureView> CompiledFrameGraph::GetView(FGTextureHandle h) const noexcept {
    if (!h || h.id == 0) return {};
    const u32 idx = h.id - 1;
    if (idx >= (u32)mViews.size()) return {};
    return mViews[idx];
}

ref<Texture> CompiledFrameGraph::GetTexture(FGTextureHandle h) const noexcept {
    if (!h || h.id == 0) return {};
    const u32 idx = h.id - 1;
    if (idx >= (u32)mTextures.size()) return {};
    return mTextures[idx];
}

// ---------------- PassBuilder ----------------

PassBuilder::PassBuilder(FrameGraph& fg, u32 passId) : mFG(fg), mPassId(passId) {}

PassBuilder& PassBuilder::Read(FGTextureHandle tex) {
    if (tex) mFG.mPasses[mPassId].reads.push_back(tex);
    return *this;
}

PassBuilder& PassBuilder::WriteColor(const std::string& name, TextureFormat fmt) {
    auto h = mFG.GetOrCreateResource(name, fmt, false);
    mFG.mPasses[mPassId].writesColor.push_back(h);
    return *this;
}

PassBuilder& PassBuilder::WriteDepth(const std::string& name, TextureFormat fmt) {
    auto h = mFG.GetOrCreateResource(name, fmt, true);
    mFG.mPasses[mPassId].writeDepth = h;
    return *this;
}

PassBuilder& PassBuilder::WriteSwapchain() {
    mFG.mPasses[mPassId].writesSwapchain = true;
    return *this;
}

PassBuilder& PassBuilder::ClearColor(u32 attachmentIndex, ct::gfx::ClearColor c) noexcept {
    auto& p = mFG.mPasses[mPassId];
    if (p.clearColors.size() <= attachmentIndex) p.clearColors.resize(attachmentIndex + 1);
    p.clearColors[attachmentIndex] = c;
    return *this;
}

PassBuilder& PassBuilder::ClearDepth(float depth, u32 stencil) noexcept {
    auto& p = mFG.mPasses[mPassId];
    p.hasDepthClear = true;
    p.clearDepth = depth;
    p.clearStencil = stencil;
    return *this;
}

PassBuilder& PassBuilder::Execute(std::function<void(RenderPassEncoder& pass)> fn) {
    mFG.mPasses[mPassId].exec = std::move(fn);
    return *this;
}

FGTextureHandle PassBuilder::Color(const std::string& name) const {
    auto it = mFG.mNameToHandle.find(name);
    if (it == mFG.mNameToHandle.end()) return {};
    return it->second;
}

FGTextureHandle PassBuilder::Depth(const std::string& name) const {
    auto it = mFG.mNameToHandle.find(name);
    if (it == mFG.mNameToHandle.end()) return {};
    return it->second;
}

// ---------------- FrameGraph ----------------

FrameGraph::FrameGraph(ref<Device> device) : mDevice(std::move(device)) {}

void FrameGraph::Reset() {
    mPasses.clear();
    mResources.clear();
    mNameToHandle.clear();
    mPool.clear();
}

PassBuilder FrameGraph::AddPass(const std::string& name) {
    Pass p{};
    p.name = name;
    mPasses.push_back(std::move(p));
    return PassBuilder(*this, (u32)(mPasses.size() - 1));
}

FGTextureHandle FrameGraph::GetOrCreateResource(const std::string& name, TextureFormat fmt, bool depth) {
    auto it = mNameToHandle.find(name);
    if (it != mNameToHandle.end()) return it->second;

    TextureResource r{};
    r.name = name;
    r.format = fmt;
    r.isDepth = depth;
    r.imported = false;
    r.transient = true;

    mResources.push_back(std::move(r));
    FGTextureHandle h{ (u32)mResources.size() };
    mNameToHandle[name] = h;
    return h;
}

FrameGraph::TextureResource* FrameGraph::GetResource(FGTextureHandle h) {
    if (!h || h.id == 0) return nullptr;
    const u32 idx = h.id - 1;
    if (idx >= (u32)mResources.size()) return nullptr;
    return &mResources[idx];
}

// ---- Pool helpers ----

FrameGraph::PoolKey FrameGraph::MakeKey(const TextureResource& r, u32 w, u32 h) const noexcept {
    PoolKey k{};
    k.fmt = r.format;
    k.depth = r.isDepth;
    k.w = w;
    k.h = h;
    return k;
}

FrameGraph::PooledTex FrameGraph::AcquireFromPool(const PoolKey& key) noexcept {
    auto it = mPool.find(key);
    if (it == mPool.end()) return {};
    if (it->second.empty()) return {};
    auto p = it->second.back();
    it->second.pop_back();
    return p;
}

void FrameGraph::ReturnToPool(const PoolKey& key, PooledTex p) noexcept {
    if (!p.tex || !p.view) return;
    mPool[key].push_back(std::move(p));
}

// ---- Import ----

FGTextureHandle FrameGraph::ImportTexture(
    const std::string& name,
    ref<Texture> texture,
    ref<TextureView> view,
    TextureFormat fmt,
    bool depth)
{
    if (!texture || !view) return {};

    auto h = GetOrCreateResource(name, fmt, depth);
    auto* r = GetResource(h);
    if (!r) return {};

    r->imported = true;
    r->transient = false;
    r->format = fmt;
    r->isDepth = depth;
    r->texture = std::move(texture);
    r->view = std::move(view);
    r->width = r->texture->GetWidth();
    r->height = r->texture->GetHeight();
    return h;
}

// ---- Realize ----

bool FrameGraph::RealizeResource(TextureResource& r, u32 w, u32 h) {
    if (r.imported) {
        return (r.texture && r.view);
    }

    if (w == 0 || h == 0) return false;

    if (r.texture && r.view && r.width == w && r.height == h) return true;

    if (r.transient) {
        const auto key = MakeKey(r, w, h);
        auto pooled = AcquireFromPool(key);
        if (pooled.tex && pooled.view) {
            r.texture = std::move(pooled.tex);
            r.view = std::move(pooled.view);
            r.width = w;
            r.height = h;
            return true;
        }
    }

    r.width = w;
    r.height = h;
    r.view = {};
    r.texture = {};

    TextureDesc td{};
    td.width = w;
    td.height = h;
    td.mipLevels = 1;
    td.format = r.format;
    td.usage = TextureUsageFlags::RenderTarget | TextureUsageFlags::Sampled | TextureUsageFlags::CopySrc | TextureUsageFlags::CopyDst;
    td.debugName = r.name;

    auto texRes = Texture::Create(mDevice, td);
    if (!texRes) return false;
    r.texture = TRY(texRes);

    auto viewRes = r.texture->CreateView({ .debugName = r.name + ".view" });
    if (!viewRes) return false;
    r.view = TRY(viewRes);

    return true;
}

// ---- Compile ----

result<CompiledFrameGraph> FrameGraph::Compile(ref<Swapchain> swapchain) {
    if (!swapchain) return err(ErrorCode::INVALID_ARGUMENT, "FrameGraph: swapchain null");

    auto frameRes = swapchain->AcquireNextFrame();
    if (!frameRes) return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "FrameGraph: acquire failed");
    const Frame frame = TRY(frameRes);

    const u32 W = frame.width;
    const u32 H = frame.height;

    for (auto& r : mResources) {
        if (!RealizeResource(r, W, H)) {
            return err(ErrorCode::GRAPHICS_RESOURCE_CREATION_FAILED, "FrameGraph: resource realize failed");
        }
    }

    auto enc = TRY(CommandEncoder::Create(mDevice, { .debugName = "FrameGraph" }));

    for (auto& p : mPasses) {
        if (!p.exec) continue;

        RenderPassDesc rp{};
        rp.debugName = p.name;

        // Colors (MRT)
        if (p.writesSwapchain) {
            ColorAttachmentDesc c{};
            c.view = frame.colorView;
            c.clear = (p.clearColors.empty())
                ? ct::gfx::ClearColor{0.1f,0.1f,0.12f,1.0f}
                : p.clearColors[0];
            c.clearEnabled = true;
            rp.colors.push_back(c);
        } else {
            if (p.writesColor.empty()) {
                return err(ErrorCode::INVALID_STATE, "FrameGraph: pass has no color outputs");
            }

            rp.colors.reserve(p.writesColor.size());
            for (u32 i = 0; i < (u32)p.writesColor.size(); ++i) {
                auto* out = GetResource(p.writesColor[i]);
                if (!out || !out->view) return err(ErrorCode::INVALID_STATE, "FrameGraph: missing color view");

                ColorAttachmentDesc c{};
                c.view = out->view->GetNativeViewPtr();
                c.clear = (i < p.clearColors.size()) ? p.clearColors[i] : ct::gfx::ClearColor{0,0,0,1};
                c.clearEnabled = true;
                rp.colors.push_back(c);
            }
        }

        // Depth
        if (p.writeDepth) {
            auto* d = GetResource(p.writeDepth);
            if (!d || !d->view) return err(ErrorCode::INVALID_STATE, "FrameGraph: missing depth view");
            rp.enableDepth = true;
            rp.depthView = d->view->GetNativeViewPtr();
            rp.clearDepth = p.hasDepthClear ? p.clearDepth : 1.0f;
            rp.clearStencil = p.hasDepthClear ? p.clearStencil : 0;
        }

        auto pass = TRY(enc->BeginRenderPass(rp));
        p.exec(*pass);
        pass->End();
    }

    auto cmd = TRY(enc->Finish());

    CompiledFrameGraph out{};
    out.mCmd = cmd;
    out.mNameToHandle = mNameToHandle;

    out.mTextures.resize(mResources.size());
    out.mViews.resize(mResources.size());
    out.mWasTransient.resize(mResources.size(), false);

    for (u32 i = 0; i < (u32)mResources.size(); ++i) {
        out.mTextures[i] = mResources[i].texture;
        out.mViews[i] = mResources[i].view;
        out.mWasTransient[i] = (!mResources[i].imported && mResources[i].transient);
    }

    return ok(out);
}

result<ref<CommandBuffer>> FrameGraph::CompileAndRecord(ref<Swapchain> swapchain) {
    auto compiled = TRY(Compile(swapchain));
    return ok(compiled.GetCommandBuffer());
}

void FrameGraph::Recycle(const CompiledFrameGraph& compiled) noexcept {
    for (u32 i = 0; i < (u32)mResources.size() && i < (u32)compiled.mWasTransient.size(); ++i) {
        if (!compiled.mWasTransient[i]) continue;

        auto tex = compiled.mTextures[i];
        auto view = compiled.mViews[i];
        if (!tex || !view) continue;

        const auto& r = mResources[i];
        PoolKey key{};
        key.fmt = r.format;
        key.depth = r.isDepth;
        key.w = tex->GetWidth();
        key.h = tex->GetHeight();

        ReturnToPool(key, { tex, view });
    }
}

} // namespace ct::gfx
