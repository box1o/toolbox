#include "studio/core/application.hpp"
#include "toolbox/gfx/events/input/events.hpp"

namespace ct::studio {
Application::Application() {
    mWindow = TRY(ct::gfx::Window::Create(ct::gfx::WindowInfo{
        .title = "studio",
        .floating = true,
    }));

    mWindow->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

    ct::gfx::RendererDesc rDesc{};
    rDesc.enableShadows = true;

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
    mRenderer->OnEvent(event);

    events::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<events::KeyPressedEvent>([&](const events::KeyPressedEvent& ev) {
        // log::Info("Event received: {}", events::ToString(ev));
        // log::Info("{}", static_cast<char>(ev.key));
        return true;
    });

    // dispatcher.Dispatch<events::KeyPressedEvent>(...);
    // dispatcher.Dispatch<events::MouseMovedEvent>(...);
}

} // namespace ct::studio
