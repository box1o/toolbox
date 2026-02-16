#include "studio/core/application.hpp"
#include <toolbox/base/base.hpp>
#include <toolbox/gfx/gfx.hpp>

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#endif

using namespace ct;
int main(int argc, char* argv[]) {
    log::Configure("toolbox");

    studio::Application app;

#if defined(__EMSCRIPTEN__)
    emscripten_set_main_loop_arg(
        [](void* arg) {
            auto* app = static_cast<studio::Application*>(arg);
            if (app->Update()) {
                emscripten_cancel_main_loop();
            }
        },
        &app, 0, true);
#else
    while (true) {
        if (app.Update()) {
            break;
        }
    }
#endif
}
