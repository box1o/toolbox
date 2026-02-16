#include "toolbox/base/base.hpp"
#include "toolbox/gfx/gfx.hpp"


class Application
{

public:
    Application();
    ~Application();

    void Run();
    void Spin();


private:

    ct::ref<ct::Window> mWindow;
    ct::ref<ct::Device> mDevice;
    ct::ref<ct::Surface> mSurface;

};
