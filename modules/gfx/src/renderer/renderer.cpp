#include "../../include/toolbox/gfx/renderer/renderer.hpp"
#include "toolbox/base/errors/result.hpp"
#include "toolbox/gfx/api/device.hpp"

namespace ct::gfx {

Renderer::Renderer(ref<Window> window, const RendererDesc& desc) : mWindow(window) {}
Renderer::~Renderer() {}

result<void> Renderer::Initialize() {
    //NOTE: Create Device
    mDevice = TRY_RETURN(Device::Create({}));
    log::Info("Renderer: Device created successfully");

    //NOTE: Create Surface
    mSurface = TRY_RETURN(Surface::Create(mDevice, mWindow));
    log::Info("Renderer: Surface created successfully");

    mSwapchain = TRY_RETURN(Swapchain::Create(mDevice, mSurface));
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

} // namespace ct::gfx
