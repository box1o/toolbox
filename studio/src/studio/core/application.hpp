#pragma once

#include <toolbox/base/base.hpp>
#include <toolbox/gfx/gfx.hpp>

using namespace ct;

namespace studio {

class Application {
public:
    Application();
    ~Application();

    bool Update();

private:
    ref<Window> mWindow;
    ref<Device> mDevice;
    ref<Surface> mSurface;
};

} // namespace studio
