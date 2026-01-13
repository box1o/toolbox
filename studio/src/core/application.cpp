#include "application.hpp"
#include "ct/base/logger/logger.hpp"
#include <cstdlib>



namespace ct {

Application::Application(const ApplicationInfo& info)
:mInfo(info){
    log::Configure("studio", ct::log::Level::Trace);

    gfx::WindowInfo wInfo{ };
    mWindow = createScope<gfx::Window>(wInfo);



    //Create device
    gfx::DeviceInfo dInfo{};
    dInfo.validate = true;
    dInfo.backend = gfx::DeviceBackend::Vulkan;
    auto res = gfx::Device::Create(dInfo); 
    if (!res) {
        log::Error("Failed to create device: {}", res.error().Message());
        std::abort();
    }
    mDevice = res.value();



}
Application::~Application(){}


void Application::Run(){
    while(mRunning){
        // log::Info("Application running...");

    }
}


}



