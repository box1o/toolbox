#pragma once

#include <toolbox/base/base.hpp>
#include <toolbox/gfx/gfx.hpp>
#include <toolbox/gfx/renderer/renderer.hpp>

namespace ct::studio {

class Application final {
public:
    Application();
    ~Application();

    bool Update();

    void OnEvent(ct::events::EventBase& event);

private:
    scope<gfx::Renderer> mRenderer{nullptr};
    ref<gfx::Window> mWindow{nullptr};
};

} // namespace ct::studio
