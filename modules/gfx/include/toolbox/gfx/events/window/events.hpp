#pragma once
#include "../base.hpp"

namespace ct::events {

struct WindowCloseEvent : EventBase {
    WindowCloseEvent() = default;

    EVENT_TYPE(WindowClose)
    EVENT_CATEGORY(EventCategory::Window)
};

struct WindowResizeEvent : EventBase {
    u32 width;
    u32 height;

    WindowResizeEvent(u32 w, u32 h) : width(w), height(h) {}

    EVENT_TYPE(WindowResize)
    EVENT_CATEGORY(EventCategory::Window)
};

struct WindowFocusEvent : EventBase {
    WindowFocusEvent() = default;

    EVENT_TYPE(WindowFocus)
    EVENT_CATEGORY(EventCategory::Window)
};

struct WindowLostFocusEvent : EventBase {
    WindowLostFocusEvent() = default;

    EVENT_TYPE(WindowLostFocus)
    EVENT_CATEGORY(EventCategory::Window)
};

struct WindowMovedEvent : EventBase {
    i32 x;
    i32 y;

    WindowMovedEvent(i32 x_, i32 y_) : x(x_), y(y_) {}

    EVENT_TYPE(WindowMoved)
    EVENT_CATEGORY(EventCategory::Window)
};

struct WindowMinimizedEvent : EventBase {
    WindowMinimizedEvent() = default;

    EVENT_TYPE(WindowMinimized)
    EVENT_CATEGORY(EventCategory::Window)
};

struct WindowMaximizedEvent : EventBase {
    WindowMaximizedEvent() = default;

    EVENT_TYPE(WindowMaximized)
    EVENT_CATEGORY(EventCategory::Window)
};

struct WindowRestoredEvent : EventBase {
    WindowRestoredEvent() = default;

    EVENT_TYPE(WindowRestored)
    EVENT_CATEGORY(EventCategory::Window)
};

} // namespace ct::events
