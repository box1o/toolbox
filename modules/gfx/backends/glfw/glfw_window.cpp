#include "glfw_window.hpp"


namespace ct::gfx {


GLFWwindowImpl::GLFWwindowImpl(const WindowInfo& info) {
    if (!glfwInit()) {
        log::Error("Failed to initialize GLFW");
        return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, info.resizable ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, info.decorated ? GLFW_TRUE : GLFW_FALSE);

    mWindow = glfwCreateWindow(static_cast<int>(info.width),
                               static_cast<int>(info.height),
                               info.title.c_str(),
                               info.fullscreen ? glfwGetPrimaryMonitor() : nullptr,
                               nullptr);
    if (!mWindow) {
        log::Error("Failed to create GLFW window");
        glfwTerminate();
    }
}


GLFWwindowImpl::~GLFWwindowImpl() {
    if (mWindow) {
        glfwDestroyWindow(mWindow);
        mWindow = nullptr;
    }
    glfwTerminate();
}


bool GLFWwindowImpl::ShouldClose() const noexcept {
    return glfwWindowShouldClose(mWindow);
}


void GLFWwindowImpl::PollEvents() const noexcept {
    glfwPollEvents();
}

}




