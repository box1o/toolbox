#pragma once
#include <toolbox/base/base.hpp>

namespace ct::events {

enum class EventType : u32 {
    None = 0,

    WindowClose = 1,
    WindowResize = 2,
    WindowFocus = 3,
    WindowLostFocus = 4,
    WindowMoved = 5,
    WindowMinimized = 6,
    WindowMaximized = 7,
    WindowRestored = 8,

    KeyPressed = 100,
    KeyReleased = 101,
    KeyTyped = 102,

    MouseMoved = 150,
    MouseScrolled = 151,
    MouseButtonPressed = 152,
    MouseButtonReleased = 153,
    MouseButtonClicked = 154,
    MouseEntered = 155,
    MouseLeft = 156,

    FrameBegin = 200,
    FrameEnd = 201,
    RenderBegin = 202,
    RenderEnd = 203,
    ViewportResize = 204,
    SwapBuffers = 205,

    AppTick = 300,
    AppUpdate = 301,
    AppRender = 302,
    AppShutdown = 303,
    AppSuspend = 304,
    AppResume = 305,

    Custom = 10000
};

} // namespace ct::events
