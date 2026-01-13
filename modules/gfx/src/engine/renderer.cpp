#include "ct/gfx/engine/renderer.hpp"

namespace ct::gfx {


Renderer::Renderer(const RendererInfo& info)
: mInfo(info) {
    gfx::WindowInfo wInfo{ };
    mWindow = createScope<gfx::Window>(wInfo);

    //Create device
    gfx::DeviceInfo dInfo{};
    dInfo.validate = true;
    dInfo.verbose = false;
    dInfo.backend = gfx::DeviceBackend::Vulkan;
    auto res = gfx::Device::Create(dInfo); 
    if (!res) {
        log::Error("Failed to create device: {}", res.error().Message());
        std::abort();
    }
    mDevice = res.value();

    // VkInstance instance = static_cast<VkInstance>(mDevice->GetInstanceHandle());

}



void Renderer::Begin(const ref<Camera>& camera){}
void Renderer::End(){}



}
