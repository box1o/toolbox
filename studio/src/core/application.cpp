#include "application.hpp"
#include "ct/base/logger/logger.hpp"
#include "ct/base/types/types.hpp"
#include "ct/gfx/engine/camera.hpp"
#include "ct/gfx/engine/renderer.hpp"

#include <vulkan/vulkan.h>


namespace ct {


Application::Application(const ApplicationInfo& info)
:mInfo(info){
    log::Configure("studio", ct::log::Level::Trace);
    gfx::RendererInfo rInfo{};
    mRenderer = createScope<gfx::Renderer>(rInfo);




    gfx::CameraInfo cInfo{};
    cInfo.name = "main";
    mCamera = createRef<gfx::Camera>(cInfo);


}
Application::~Application(){}


void Application::Run(){
    while(mRunning){
        mRenderer->Begin(mCamera);


        mRenderer->End();
    }
}


}



