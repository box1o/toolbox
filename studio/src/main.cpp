#include "ct/gfx/api/device.hpp"
#include <ct/base/base.hpp>
#include <ct/gfx/gfx.hpp>



using namespace ct;

int main(int /*argc*/, char* /*argv*/[]) {
    log::Configure("studio", ct::log::Level::Trace);


    auto window = gfx::Window();

    auto res = gfx::Device::Create({
        .backend = gfx::DeviceBackend::Vulkan,
    }); 
    if (!res) {
        log::Error("Failed to create device: {}", res.error().Message());
        return -1;
    }
    auto device = res.value();



    // same api for the other things like swapchain 
    // create the Info struct 
    // {
    //     pView // pointer the the texture to display ... 
    //     //not shure about other settings
    //
    // }
    // to create use gfx::Swapchain::Create(device, {info});
    // same for the resources buffers .. also take a shared ptr to the device








    return 0;
}
