#include "../../include/toolbox/gfx/renderer/renderer.hpp"
#include "toolbox/base/errors/result.hpp"

namespace ct::gfx {

Renderer::Renderer(ref<Window> window, const RendererDesc& desc) : mWindow(window) {}
Renderer::~Renderer() {}

result<void> Renderer::Initialize() {
    // NOTE: Create Device
    mDevice = TRY_RETURN(Device::Create({}));
    log::Info("maxColorAttachments: {}", mDevice->GetLimits().maxColorAttachments);

    mSwapchain = TRY_RETURN(Swapchain::Create(mDevice, mWindow));
    log::Info("Renderer: Swapchain created successfully");

    return ok();
}

void Renderer::BeginFrame(/*const Camera& camera,*/ f32 deltaTime) {}
void Renderer::EndFrame() {
    mDevice->Tick();
    auto frame = mSwapchain->AcquireNextFrame();

    TRY_VOID(mSwapchain->Present());
}

// void Submit();

void Renderer::OnResize(u32 width, u32 height) noexcept {
    mSwapchain->Resize(width, height);
}

} // namespace ct::gfx
