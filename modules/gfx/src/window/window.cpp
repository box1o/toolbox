#include <ct/gfx/window/window.hpp>
#include <ct/base/logger/logger.hpp>


namespace ct::gfx {


Window::Window(const WindowInfo& /*info*/){

    if (!glfwInit()) {
        log::Critical("GLFW init failed");
        std::abort();
    }

}


} // namespace cc::gfx
