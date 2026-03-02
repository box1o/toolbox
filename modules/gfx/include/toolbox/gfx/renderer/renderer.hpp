#pragma once
#include <toolbox/base/base.hpp>

#include "../api/device.hpp"
#include "../api/swapchain.hpp"
#include "../api/queue.hpp"
#include "../api/shader.hpp"
#include "../api/pipeline.hpp"
#include "../api/buffer.hpp"
#include "../api/command_encoder.hpp"
#include "../window/window.hpp"

namespace ct::gfx {

struct RendererDesc {
    u32 maxDrawCalls{100000};
    u32 maxInstances{500000};
    u32 shadowMapSize{2048};
    bool enableShadows{true};
    bool enableInstancing{true};
    bool enableFrustumCulling{true};
};

class Renderer {
public:
    Renderer(ref<Window> window, const RendererDesc& desc);
    ~Renderer();

    result<void> Initialize();

    void BeginFrame(f32 deltaTime);
    void EndFrame();

    void OnResize(u32 width, u32 height) noexcept;

private:
    result<void> CreateTriangleResources();

    ref<Window> mWindow{nullptr};
    ref<Device> mDevice{nullptr};
    ref<Swapchain> mSwapchain{nullptr};
    ref<Queue> mQueue{nullptr};

    ref<Shader> mShader{nullptr};
    ref<Pipeline> mPipeline{nullptr};
    ref<Buffer> mVertexBuffer{nullptr};

    Frame mCurrentFrame{};
    bool mFrameActive{false};
};

} // namespace ct::gfx
