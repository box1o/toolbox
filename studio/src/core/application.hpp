#pragma once
#include "ct/gfx/engine/camera.hpp"
#include "ct/gfx/engine/renderer.hpp"
#include <string>


namespace ct {

struct ApplicationInfo{
    std::string name{"studio"};

};

class Application{
public:
    Application(const ApplicationInfo& info);
    ~Application();

    void Run();

private:
    ApplicationInfo  mInfo;
    scope<gfx::Renderer> mRenderer;
    ref<gfx::Camera> mCamera;

    bool mRunning{true};



};

}
