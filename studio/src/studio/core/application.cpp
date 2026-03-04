#include "studio/core/application.hpp"

#include "toolbox/gfx/events/input/events.hpp"

namespace ct::studio {

Application::Application() {
    mWindow = TRY(gfx::Window::Create(gfx::WindowInfo{
        .title = "studio",
        .floating = true,
    }));

    mWindow->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

    gfx::RendererDesc rDesc{};
    rDesc.enableShadows = true;

    mRenderer = createScope<gfx::Renderer>(mWindow, rDesc);
    TRY_VOID(mRenderer->Initialize());


    // auto unlitHandle = mRenderer->GetShaders().GetBuiltin(gfx::BuiltinShader::Unlit);

    mEditorCamera = EditorCamera();
}

Application::~Application() = default;

bool Application::Update() {
    if (!mWindow) return false;
    mWindow->PollEvents();
    if (mWindow->ShouldClose()) return true;

    mEditorCamera.Tick(0.016f);

    mRenderer->BeginFrame(mEditorCamera.GetCamera(), 0.016f);
    mRenderer->EndFrame();

    return false;
}

void Application::OnEvent(events::EventBase& event) {

    mEditorCamera.OnEvent(event);
    mRenderer->OnEvent(event);

    events::EventDispatcher dispatcher(event);

    dispatcher.Dispatch<events::WindowResizeEvent>([&](const events::WindowResizeEvent& ev) {
        mEditorCamera.SetViewportSize(ev.width, ev.height);
        return false;
    });

    dispatcher.Dispatch<events::KeyPressedEvent>([&](const events::KeyPressedEvent& ev) {
        (void)ev;
        return false;
    });
}

} // namespace ct::studio
