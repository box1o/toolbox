#pragma once
#include "../base.hpp"

namespace ct::events {

enum class KeyCode : u16 {
    Space = 32,
    Apostrophe = 39,
    Comma = 44,
    Minus = 45,
    Period = 46,
    Slash = 47,

    D0 = 48,
    D1,
    D2,
    D3,
    D4,
    D5,
    D6,
    D7,
    D8,
    D9,

    Semicolon = 59,
    Equal = 61,

    A = 65,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    LeftBracket = 91,
    Backslash = 92,
    RightBracket = 93,
    GraveAccent = 96,

    Escape = 256,
    Enter,
    Tab,
    Backspace,
    Insert,
    Delete,
    Right,
    Left,
    Down,
    Up,
    PageUp,
    PageDown,
    Home,
    End,

    CapsLock = 280,
    ScrollLock,
    NumLock,
    PrintScreen,
    Pause,

    F1 = 290,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,

    LeftShift = 340,
    LeftControl,
    LeftAlt,
    LeftSuper,
    RightShift,
    RightControl,
    RightAlt,
    RightSuper,

    Menu = 348
};

enum class MouseButton : u8 {
    Left = 0,
    Right = 1,
    Middle = 2,
    Button4 = 3,
    Button5 = 4,
    Button6 = 5,
    Button7 = 6,
    Button8 = 7
};

struct KeyPressedEvent : EventBase {
    KeyCode key;
    u32 repeatCount;

    KeyPressedEvent(KeyCode k, u32 repeat = 0) : key(k), repeatCount(repeat) {}

    EVENT_TYPE(KeyPressed)
    EVENT_CATEGORY(EventCategory::Keyboard | EventCategory::Input)
};

struct KeyReleasedEvent : EventBase {
    KeyCode key;

    explicit KeyReleasedEvent(KeyCode k) : key(k) {}

    EVENT_TYPE(KeyReleased)
    EVENT_CATEGORY(EventCategory::Keyboard | EventCategory::Input)
};

struct KeyTypedEvent : EventBase {
    u32 character;

    explicit KeyTypedEvent(u32 ch) : character(ch) {}

    EVENT_TYPE(KeyTyped)
    EVENT_CATEGORY(EventCategory::Keyboard | EventCategory::Input)
};

struct MouseMovedEvent : EventBase {
    f32 x;
    f32 y;
    f32 deltaX;
    f32 deltaY;

    MouseMovedEvent(f32 x_, f32 y_, f32 dx, f32 dy) : x(x_), y(y_), deltaX(dx), deltaY(dy) {}

    EVENT_TYPE(MouseMoved)
    EVENT_CATEGORY(EventCategory::Mouse | EventCategory::Input)
};

struct MouseScrolledEvent : EventBase {
    f32 offsetX;
    f32 offsetY;

    MouseScrolledEvent(f32 x, f32 y) : offsetX(x), offsetY(y) {}

    EVENT_TYPE(MouseScrolled)
    EVENT_CATEGORY(EventCategory::Mouse | EventCategory::Input)
};

struct MouseButtonPressedEvent : EventBase {
    MouseButton button;
    f32 x;
    f32 y;

    MouseButtonPressedEvent(MouseButton btn, f32 x_, f32 y_) : button(btn), x(x_), y(y_) {}

    EVENT_TYPE(MouseButtonPressed)
    EVENT_CATEGORY(EventCategory::Mouse | EventCategory::MouseButton | EventCategory::Input)
};

struct MouseButtonReleasedEvent : EventBase {
    MouseButton button;
    f32 x;
    f32 y;

    MouseButtonReleasedEvent(MouseButton btn, f32 x_, f32 y_) : button(btn), x(x_), y(y_) {}

    EVENT_TYPE(MouseButtonReleased)
    EVENT_CATEGORY(EventCategory::Mouse | EventCategory::MouseButton | EventCategory::Input)
};

struct MouseButtonClickedEvent : EventBase {
    MouseButton button;
    f32 x;
    f32 y;

    MouseButtonClickedEvent(MouseButton btn, f32 x_, f32 y_) : button(btn), x(x_), y(y_) {}

    EVENT_TYPE(MouseButtonClicked)
    EVENT_CATEGORY(EventCategory::Mouse | EventCategory::MouseButton | EventCategory::Input)
};

struct MouseEnteredEvent : EventBase {
    MouseEnteredEvent() = default;

    EVENT_TYPE(MouseEntered)
    EVENT_CATEGORY(EventCategory::Mouse | EventCategory::Input)
};

struct MouseLeftEvent : EventBase {
    MouseLeftEvent() = default;

    EVENT_TYPE(MouseLeft)
    EVENT_CATEGORY(EventCategory::Mouse | EventCategory::Input)
};

} // namespace ct::events
