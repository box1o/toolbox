#include "toolbox/base/base.hpp"
#include "toolbox/gfx/gfx.hpp"

using namespace ct;

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    log::Configure("toolbox");

    auto window = TRY(Window::Create({.title = "toolbox", .width = 1080, .height = 720}));
    auto device = TRY(Device::Create({.validate = true, .verbose = true}));

    auto surface = TRY(ct::Surface::Create(window, device,
        {
            .presentMode = ct::PresentMode::VSync,
            .enableDepth = true,
        }));

    // auto texture =
    //     TRY(Texture::FromFile(device, "/home/toor/Pictures/screen.png", {.flipOnLoad = true}));


    while (!window->ShouldClose()) {
        window->PollEvents();

        auto frame = TRY(surface->BeginFrame());

        wgpu::CommandEncoderDescriptor encDesc{};
        auto encoder = device->GetDevice().CreateCommandEncoder(&encDesc);

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

        device->GetQueue().Submit(1, &commands);

        surface->Present();
    }
}
