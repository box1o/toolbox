#pragma once

#include <toolbox/base/base.hpp>
#include <toolbox/gfx/gfx.hpp>
#include <toolbox/gfx/renderer/renderer.hpp>
#include <toolbox/gfx/camera/editor_camera.hpp>

namespace ct::studio {

class Application final {
public:
    Application();
    ~Application();

    bool Update();
    void OnEvent(events::EventBase& event);

private:
    ref<gfx::Window> mWindow{nullptr};
    scope<gfx::Renderer> mRenderer{nullptr};
    EditorCamera mEditorCamera;
};

} // namespace ct::studio
