#include "application.hpp"

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#endif
using namespace ct;

Application::Application() {
    mWindow = TRY(ct::Window::Create({.title = "toolbox", .width = 1080, .height = 720}));
    mDevice = TRY(ct::Device::Create({.validate = true, .verbose = true}));

    mSurface = TRY(ct::Surface::Create(mWindow, mDevice,
        {
            .presentMode = ct::PresentMode::VSync,
            .enableDepth = true,
        }));
}

Application::~Application() {}

void Application::Run() {
    mWindow->PollEvents();

    auto frame = TRY(mSurface->BeginFrame());

    wgpu::CommandEncoderDescriptor encDesc{};
    auto encoder = mDevice->GetDevice().CreateCommandEncoder(&encDesc);

    wgpu::RenderPassColorAttachment colorAttachment{};
    colorAttachment.view = frame.colorView;
    colorAttachment.loadOp = wgpu::LoadOp::Clear;
    colorAttachment.storeOp = wgpu::StoreOp::Store;
    colorAttachment.clearValue = {1.0, 0.4, 0.4, 1.0};

    wgpu::RenderPassDepthStencilAttachment depthAttachment{};
    depthAttachment.view = frame.depthView;
    depthAttachment.depthLoadOp = wgpu::LoadOp::Clear;
    depthAttachment.depthStoreOp = wgpu::StoreOp::Store;
    depthAttachment.depthClearValue = 1.0f;
    // NOTE: Required when format has stencil aspect (Depth24PlusStencil8)
    depthAttachment.stencilLoadOp = wgpu::LoadOp::Clear;
    depthAttachment.stencilStoreOp = wgpu::StoreOp::Store;
    depthAttachment.stencilClearValue = 0;

    wgpu::RenderPassDescriptor passDesc{};
    passDesc.colorAttachmentCount = 1;
    passDesc.colorAttachments = &colorAttachment;
    if (frame.depthView) {
        passDesc.depthStencilAttachment = &depthAttachment;
    }

    auto pass = encoder.BeginRenderPass(&passDesc);
    // ... draw calls ...
    pass.End();

    auto commands = encoder.Finish();

    mDevice->GetQueue().Submit(1, &commands);

    mSurface->Present();
};

void Application::Spin() {
#if defined(__EMSCRIPTEN__)
    emscripten_set_main_loop_arg(
        [](void* arg) { static_cast<Application*>(arg)->Run(); }, this, 0, true);
#else

    while (!mWindow->ShouldClose()) {
        Run();
    }
#endif
}
