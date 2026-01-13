#pragma once
#include <string>

#include "ct/base/base.hpp"
#include "ct/gfx/gfx.hpp"

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
    scope<gfx::Window> mWindow;
    ref<gfx::Device> mDevice;

    bool mRunning{true};



};

}
