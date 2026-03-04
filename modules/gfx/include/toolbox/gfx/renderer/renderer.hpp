#pragma once

#include <toolbox/base/base.hpp>

#include "../api/bind_group.hpp"
#include "../api/buffer.hpp"
#include "../api/command_encoder.hpp"
#include "../api/device.hpp"
#include "../api/pipeline.hpp"
#include "../api/queue.hpp"
#include "../api/shader.hpp"
#include "../api/swapchain.hpp"
#include "../events/base.hpp"
#include "../window/window.hpp"
#include "toolbox/gfx/renderer/library/shaders.hpp"

namespace ct {
class Camera;
}

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

    // NOTE: camera is passed by const ref — renderer reads matrices, does not own it
    void BeginFrame(const Camera& camera, f32 deltaTime);
    void EndFrame();

    ShaderLibrary& GetShaders() { return *mShaderLib; }

    void OnEvent(events::EventBase& event) noexcept;

private:
    ref<Window> mWindow{nullptr};
    ref<Device> mDevice{nullptr};
    ref<Swapchain> mSwapchain{nullptr};
    ref<Queue> mQueue{nullptr};

    Frame mCurrentFrame{};
    bool mFrameActive{false};

    // NOTE: store pointer to camera passed in BeginFrame, valid until EndFrame
    const Camera* mCurrentCamera{nullptr};

    // NOTE: Libs
    scope<ShaderLibrary> mShaderLib{nullptr};
};

} // namespace ct::gfx
