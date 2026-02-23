#include "studio/core/application.hpp"

namespace ct::studio {
Application::Application() {
    mWindow = TRY(ct::gfx::Window::Create(ct::gfx::WindowInfo{
        .title = "studio",
        .floating = true,
    }));

    mWindow->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

    ct::gfx::RendererDesc rDesc{};
    rDesc.enableShadows = true;

    // Pass window via constructor as you requested
    mRenderer = createScope<ct::gfx::Renderer>(mWindow, rDesc);
    TRY_VOID(mRenderer->Initialize());
}

Application::~Application() = default;

bool Application::Update() {
    if (!mWindow) return false;
    mWindow->PollEvents();
    if (mWindow->ShouldClose()) return true;

    // Rendering code
    mRenderer->BeginFrame(/*camera,*/ 0.0f);
    // ...
    mRenderer->EndFrame();
    return false;
}

void Application::OnEvent(events::EventBase& event) {
    events::EventDispatcher dispatcher(event);
    log::Info("Event received: {}", events::ToString(event));

    dispatcher.Dispatch<events::WindowResizeEvent>([&](const events::WindowResizeEvent& ev) {
        // mRenderer->OnResize(ev.width, ev.height);
        return false;
    });

    // dispatcher.Dispatch<events::KeyPressedEvent>(...);
    // dispatcher.Dispatch<events::MouseMovedEvent>(...);
}

} // namespace ct::studio
