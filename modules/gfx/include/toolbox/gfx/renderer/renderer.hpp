#pragma once
#include <toolbox/base/base.hpp>

#include "../api/device.hpp"
#include "../api/swapchain.hpp"
#include "../api/queue.hpp"
#include "../window/window.hpp"

namespace ct::gfx {

struct RendererDesc {
    u32 maxDrawCalls = 100000;
    u32 maxInstances = 500000;
    u32 shadowMapSize = 2048;
    bool enableShadows = true;
    bool enableInstancing = true;
    bool enableFrustumCulling = true;
};

class Renderer {
public:
    Renderer(ref<Window> window, const RendererDesc& desc);
    ~Renderer();
    result<void> Initialize();

    void BeginFrame(/*const Camera& camera,*/ f32 deltaTime);
    void EndFrame();

    void OnResize(u32 width, u32 height) noexcept;
    // void Submit();

private:
    ref<Window> mWindow{nullptr};
    ref<Device> mDevice{nullptr};
    ref<Swapchain> mSwapchain{nullptr};
    ref<Queue> mQueue{nullptr};
};

} // namespace ct::gfx
