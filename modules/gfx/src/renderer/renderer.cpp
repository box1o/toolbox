#include "../../include/toolbox/gfx/renderer/renderer.hpp"

#include "toolbox/base/logger/logger.hpp"
#include "toolbox/base/types/types.hpp"
#include "toolbox/gfx/events/dispatcher.hpp"
#include "toolbox/gfx/events/window/events.hpp"
#include "toolbox/gfx/renderer/library/shaders.hpp"

namespace ct::gfx {

Renderer::Renderer(ref<Window> window, const RendererDesc& desc) : mWindow(std::move(window)) {
    (void)desc;
}

Renderer::~Renderer() = default;

result<void> Renderer::Initialize() {
    mDevice = TRY_RETURN(Device::Create({}));
    mSwapchain = TRY_RETURN(Swapchain::Create(mDevice, mWindow));
    mQueue = TRY_RETURN(Queue::Create(mDevice));

    mShaderLib = createScope<ShaderLibrary>(mDevice);

    return ok();
}

void Renderer::BeginFrame(const Camera& camera, f32 deltaTime) {
    (void)deltaTime;
    mCurrentCamera = &camera;
    mDevice->Tick();

    auto frameResult = mSwapchain->AcquireNextFrame();
    if (!frameResult) {
        log::Error("Renderer: failed to acquire frame");
        mFrameActive = false;
        return;
    }

    mCurrentFrame = frameResult.value();
    mFrameActive = true;
}

void Renderer::EndFrame() {
    if (!mFrameActive) {
        log::Warn("you need to begin frame before you end it ");
        return;
    }
    mFrameActive = false;

    auto encoderResult = CommandEncoder::Create(mDevice, {.debugName = "frame_encoder"});
    if (!encoderResult) {
        log::Error("Renderer: failed to create command encoder");
        return;
    }
    auto encoder = encoderResult.value();

    RenderPassDesc rpDesc{};
    rpDesc.debugName = "main_pass";

    ColorAttachmentDesc colorAttachment{};
    colorAttachment.view = mCurrentFrame.colorView;
    colorAttachment.clear = {0.1f, 0.1f, 0.1f, 1.0f};
    colorAttachment.clearEnabled = true;
    rpDesc.colors.push_back(colorAttachment);

    rpDesc.enableDepth = true;
    rpDesc.depthView = mCurrentFrame.depthView;
    rpDesc.clearDepth = 1.0f;
    rpDesc.clearStencil = 0;

    auto passResult = encoder->BeginRenderPass(rpDesc);
    if (!passResult) {
        log::Error("Renderer: failed to begin render pass");
        return;
    }
    auto pass = passResult.value();

    pass->End();

    auto cmdResult = encoder->Finish({.debugName = "frame_commands"});
    if (!cmdResult) {
        log::Error("Renderer: failed to finish command encoder");
        return;
    }
    auto cmdBuffer = cmdResult.value();

    auto submitResult = mQueue->Submit({cmdBuffer});
    if (!submitResult) {
        log::Error("Renderer: failed to submit command buffer");
        return;
    }

    auto presentResult = mSwapchain->Present();
    if (!presentResult) {
        log::Error("Renderer: failed to present");
    }

    mCurrentCamera = nullptr;
}

void Renderer::OnEvent(events::EventBase& event) noexcept {
    events::EventDispatcher dispatcher(event);

    dispatcher.Dispatch<events::WindowResizeEvent>([&](const events::WindowResizeEvent& ev) {
        // NOTE: Resize swapchain
        if (!mSwapchain) return false;
        mSwapchain->Resize(ev.width, ev.height);

        return false;
    });
}

} // namespace ct::gfx
