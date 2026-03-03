#pragma once
#include <toolbox/base/base.hpp>

#include "../api/device.hpp"
#include "../api/swapchain.hpp"
#include "../api/queue.hpp"
#include "../api/shader.hpp"
#include "../api/pipeline.hpp"
#include "../api/buffer.hpp"
#include "../api/bind_group.hpp"
#include "../api/command_encoder.hpp"
#include "../events/base.hpp"
#include "../events/dispatcher.hpp"
#include "../events/input/events.hpp"
#include "../events/window/events.hpp"
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

    void OnEvent(events::EventBase& event) noexcept;
    void OnResize(u32 width, u32 height) noexcept;

private:
    ref<Window> mWindow{nullptr};
    ref<Device> mDevice{nullptr};
    ref<Swapchain> mSwapchain{nullptr};
    ref<Queue> mQueue{nullptr};

    ref<Shader> mShader{nullptr};
    ref<Pipeline> mPipeline{nullptr};
    ref<Buffer> mVertexBuffer{nullptr};
    ref<Buffer> mIndexBuffer{nullptr};
    ref<Buffer> mUniformBuffer{nullptr};
    ref<BindGroup> mSceneBindGroup{nullptr};

    Frame mCurrentFrame{};
    bool mFrameActive{false};
    u32 mIndexCount{0};

    f32 mOrbitYaw{0.7f};
    f32 mOrbitPitch{0.35f};
    f32 mOrbitDistance{3.0f};
    bool mOrbitDragging{false};
    f32 mLastMouseX{0.0f};
    f32 mLastMouseY{0.0f};
};

} // namespace ct::gfx
