#include "ct/gfx/window/window.hpp"

#include <GLFW/glfw3.h>

namespace ct::gfx {

class GLFWwindowImpl final : public Window{
public:
    GLFWwindowImpl(const WindowInfo& info);
    ~GLFWwindowImpl() override;

    void* GetWindowHandle() const noexcept override{return mWindow;}
    bool ShouldClose() const noexcept override;

    void PollEvents() const noexcept override;
private:

    GLFWwindow* mWindow{nullptr};


};

}

