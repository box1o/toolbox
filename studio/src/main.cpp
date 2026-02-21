#include <toolbox/base/base.hpp>
#include <toolbox/gfx/gfx.hpp>

#include <webgpu/webgpu_cpp.h>

using namespace ct;
int main(int argc, char* argv[]) {
    log::Configure("toolbox");
    log::Info("Starting application");

    auto window = TRY(gfx::Window::Create({}));
    auto device = TRY(gfx::Device::Create({
        .powerPreference = gfx::PowerPreference::HighPerformance,
    }));
    auto queue = TRY(gfx::Queue::Create(device, {}));
    auto surface = TRY(gfx::Surface::Create(device, window));
    // auto cmd = TRY(gfx::CommandBuffer::Create({}));

    auto swapchain = TRY(gfx::Swapchain::Create(device, window));


    auto cmd = TRY(gfx::CommandBuffer::Create({}));

    while (!window->ShouldClose()) {
        window->PollEvents();
        device->Tick();

        auto frame = swapchain->AcquireNextFrame();

        // cmd stuff
        //  queue->Submit({cmd});

        swapchain->Present();
    }
}
