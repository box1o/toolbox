#pragma once
#include <toolbox/base/base.hpp>

namespace ct::events {

enum class EventCategory : u16 {
    None        = 0,
    Window      = 1 << 0,
    Input       = 1 << 1,
    Keyboard    = 1 << 2,
    Mouse       = 1 << 3,
    MouseButton = 1 << 4,
    Render      = 1 << 5,
    Application = 1 << 6,
    Scene       = 1 << 7,
    Physics     = 1 << 8,
    Audio       = 1 << 9,
    Custom      = 1 << 15
};

constexpr EventCategory operator|(EventCategory a, EventCategory b) {
    return static_cast<EventCategory>(static_cast<u16>(a) | static_cast<u16>(b));
}

constexpr u16 operator&(EventCategory a, EventCategory b) {
    return static_cast<u16>(a) & static_cast<u16>(b);
}

constexpr u16 operator&(u16 a, EventCategory b) { return a & static_cast<u16>(b); }

} // namespace ct::events
