#include "studio/core/application.hpp"

namespace studio {
Application::Application() {
    mWindow = TRY(Window::Create());
    mDevice = TRY(Device::Create());
    mSurface = TRY(Surface::Create(mWindow, mDevice, {}));
}

Application::~Application() {}

bool Application::Update() {
    mWindow->PollEvents();
    if (mWindow->ShouldClose()) {
        return true;
    }

    auto frame = TRY(mSurface->BeginFrame());

    // Create encoder
    wgpu::CommandEncoderDescriptor encDesc{};
    auto encoder = mDevice->GetDevice().CreateCommandEncoder(&encDesc);

    // Create render pass
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
    depthAttachment.stencilLoadOp = wgpu::LoadOp::Clear;
    depthAttachment.stencilStoreOp = wgpu::StoreOp::Store;
    depthAttachment.stencilClearValue = 0;

    wgpu::RenderPassDescriptor passDesc{};
    passDesc.colorAttachmentCount = 1;
    passDesc.colorAttachments = &colorAttachment;
    if (frame.depthView) {
        passDesc.depthStencilAttachment = &depthAttachment;
    }

    // Create render pass
    auto pass = encoder.BeginRenderPass(&passDesc);
    pass.End();

    auto commands = encoder.Finish();
    mDevice->GetQueue().Submit(1, &commands);

    // Present
    mSurface->Present();
    return false;
}

} // namespace studio
