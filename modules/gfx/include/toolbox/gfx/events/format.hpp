#pragma once
#include <string>
#include <string_view>

#include <toolbox/base/base.hpp>
#include <toolbox/gfx/events/base.hpp>
#include <toolbox/gfx/events/type.hpp>
#include <toolbox/gfx/events/category.hpp>

#include <toolbox/gfx/events/application/events.hpp>
#include <toolbox/gfx/events/input/events.hpp>
#include <toolbox/gfx/events/renderer/events.hpp>
#include <toolbox/gfx/events/window/events.hpp>

#ifndef EMSCRIPTEN
#include <fmt/core.h>
#include <fmt/format.h>
#endif

namespace ct::events {

inline constexpr std::string_view ToString(EventType t) noexcept {
    switch (t) {
    case EventType::None: return "None";

    case EventType::WindowClose: return "WindowClose";
    case EventType::WindowResize: return "WindowResize";
    case EventType::WindowFocus: return "WindowFocus";
    case EventType::WindowLostFocus: return "WindowLostFocus";
    case EventType::WindowMoved: return "WindowMoved";
    case EventType::WindowMinimized: return "WindowMinimized";
    case EventType::WindowMaximized: return "WindowMaximized";
    case EventType::WindowRestored: return "WindowRestored";

    case EventType::KeyPressed: return "KeyPressed";
    case EventType::KeyReleased: return "KeyReleased";
    case EventType::KeyTyped: return "KeyTyped";

    case EventType::MouseMoved: return "MouseMoved";
    case EventType::MouseScrolled: return "MouseScrolled";
    case EventType::MouseButtonPressed: return "MouseButtonPressed";
    case EventType::MouseButtonReleased: return "MouseButtonReleased";
    case EventType::MouseButtonClicked: return "MouseButtonClicked";
    case EventType::MouseEntered: return "MouseEntered";
    case EventType::MouseLeft: return "MouseLeft";

    case EventType::FrameBegin: return "FrameBegin";
    case EventType::FrameEnd: return "FrameEnd";
    case EventType::RenderBegin: return "RenderBegin";
    case EventType::RenderEnd: return "RenderEnd";
    case EventType::ViewportResize: return "ViewportResize";
    case EventType::SwapBuffers: return "SwapBuffers";

    case EventType::AppTick: return "AppTick";
    case EventType::AppUpdate: return "AppUpdate";
    case EventType::AppRender: return "AppRender";
    case EventType::AppShutdown: return "AppShutdown";
    case EventType::AppSuspend: return "AppSuspend";
    case EventType::AppResume: return "AppResume";

    case EventType::Custom: return "Custom";
    default: return "Unknown";
    }
}

inline std::string CategoryFlagsToString(u16 flags) {
    if (flags == static_cast<u16>(EventCategory::None)) return "None";

    struct Entry { EventCategory cat; std::string_view name; };
    constexpr Entry entries[] = {
        {EventCategory::Window,      "Window"},
        {EventCategory::Input,       "Input"},
        {EventCategory::Keyboard,    "Keyboard"},
        {EventCategory::Mouse,       "Mouse"},
        {EventCategory::MouseButton, "MouseButton"},
        {EventCategory::Render,      "Render"},
        {EventCategory::Application, "Application"},
        {EventCategory::Scene,       "Scene"},
        {EventCategory::Physics,     "Physics"},
        {EventCategory::Audio,       "Audio"},
        {EventCategory::Custom,      "Custom"},
    };

    std::string out;
    bool first = true;
    for (const auto& e : entries) {
        if (flags & e.cat) {
            if (!first) out += '|';
            out += e.name;
            first = false;
        }
    }
    return out.empty() ? std::string{"None"} : out;
}

inline std::string ToString(const EventBase& e) {
#ifndef EMSCRIPTEN
    auto s = fmt::format("{} [type={}, cat={}, handled={}, t={:.6f}]",
        e.GetName(),
        ToString(e.GetEventType()),
        CategoryFlagsToString(e.GetCategoryFlags()),
        e.handled ? "true" : "false",
        e.timestamp);
#else
    std::string s;
    s.reserve(128);
    s += std::string(e.GetName());
    s += " [type=";
    s += std::string(ToString(e.GetEventType()));
    s += ", cat=";
    s += CategoryFlagsToString(e.GetCategoryFlags());
    s += ", handled=";
    s += (e.handled ? "true" : "false");
    s += ", t=";
    s += std::to_string(e.timestamp);
    s += "]";
#endif

    switch (e.GetEventType()) {
    case EventType::WindowResize: {
        const auto& ev = static_cast<const WindowResizeEvent&>(e);
#ifndef EMSCRIPTEN
        s += fmt::format(" ({}x{})", ev.width, ev.height);
#else
        s += " (" + std::to_string(ev.width) + "x" + std::to_string(ev.height) + ")";
#endif
        break;
    }
    case EventType::WindowMoved: {
        const auto& ev = static_cast<const WindowMovedEvent&>(e);
#ifndef EMSCRIPTEN
        s += fmt::format(" (x={}, y={})", ev.x, ev.y);
#else
        s += " (x=" + std::to_string(ev.x) + ", y=" + std::to_string(ev.y) + ")";
#endif
        break;
    }
    case EventType::KeyPressed: {
        const auto& ev = static_cast<const KeyPressedEvent&>(e);
#ifndef EMSCRIPTEN
        s += fmt::format(" (key={}, repeat={})", static_cast<u16>(ev.key), ev.repeatCount);
#else
        s += " (key=" + std::to_string(static_cast<u16>(ev.key)) +
             ", repeat=" + std::to_string(ev.repeatCount) + ")";
#endif
        break;
    }
    case EventType::KeyReleased: {
        const auto& ev = static_cast<const KeyReleasedEvent&>(e);
#ifndef EMSCRIPTEN
        s += fmt::format(" (key={})", static_cast<u16>(ev.key));
#else
        s += " (key=" + std::to_string(static_cast<u16>(ev.key)) + ")";
#endif
        break;
    }
    case EventType::KeyTyped: {
        const auto& ev = static_cast<const KeyTypedEvent&>(e);
#ifndef EMSCRIPTEN
        s += fmt::format(" (codepoint={})", ev.character);
#else
        s += " (codepoint=" + std::to_string(ev.character) + ")";
#endif
        break;
    }
    case EventType::MouseMoved: {
        const auto& ev = static_cast<const MouseMovedEvent&>(e);
#ifndef EMSCRIPTEN
        s += fmt::format(" (x={:.2f}, y={:.2f}, dx={:.2f}, dy={:.2f})",
            ev.x, ev.y, ev.deltaX, ev.deltaY);
#else
        s += " (x=" + std::to_string(ev.x) + ", y=" + std::to_string(ev.y) +
             ", dx=" + std::to_string(ev.deltaX) + ", dy=" + std::to_string(ev.deltaY) + ")";
#endif
        break;
    }
    case EventType::MouseScrolled: {
        const auto& ev = static_cast<const MouseScrolledEvent&>(e);
#ifndef EMSCRIPTEN
        s += fmt::format(" (ox={:.2f}, oy={:.2f})", ev.offsetX, ev.offsetY);
#else
        s += " (ox=" + std::to_string(ev.offsetX) + ", oy=" + std::to_string(ev.offsetY) + ")";
#endif
        break;
    }
    case EventType::MouseButtonPressed: {
        const auto& ev = static_cast<const MouseButtonPressedEvent&>(e);
#ifndef EMSCRIPTEN
        s += fmt::format(" (btn={}, x={:.2f}, y={:.2f})", static_cast<u8>(ev.button), ev.x, ev.y);
#else
        s += " (btn=" + std::to_string(static_cast<u8>(ev.button)) +
             ", x=" + std::to_string(ev.x) + ", y=" + std::to_string(ev.y) + ")";
#endif
        break;
    }
    case EventType::MouseButtonReleased: {
        const auto& ev = static_cast<const MouseButtonReleasedEvent&>(e);
#ifndef EMSCRIPTEN
        s += fmt::format(" (btn={}, x={:.2f}, y={:.2f})", static_cast<u8>(ev.button), ev.x, ev.y);
#else
        s += " (btn=" + std::to_string(static_cast<u8>(ev.button)) +
             ", x=" + std::to_string(ev.x) + ", y=" + std::to_string(ev.y) + ")";
#endif
        break;
    }
    case EventType::MouseButtonClicked: {
        const auto& ev = static_cast<const MouseButtonClickedEvent&>(e);
#ifndef EMSCRIPTEN
        s += fmt::format(" (btn={}, x={:.2f}, y={:.2f})", static_cast<u8>(ev.button), ev.x, ev.y);
#else
        s += " (btn=" + std::to_string(static_cast<u8>(ev.button)) +
             ", x=" + std::to_string(ev.x) + ", y=" + std::to_string(ev.y) + ")";
#endif
        break;
    }
    case EventType::FrameBegin: {
        const auto& ev = static_cast<const FrameBeginEvent&>(e);
#ifndef EMSCRIPTEN
        s += fmt::format(" (dt={:.4f})", ev.deltaTime);
#else
        s += " (dt=" + std::to_string(ev.deltaTime) + ")";
#endif
        break;
    }
    case EventType::FrameEnd: {
        const auto& ev = static_cast<const FrameEndEvent&>(e);
#ifndef EMSCRIPTEN
        s += fmt::format(" (frameTime={:.4f})", ev.frameTime);
#else
        s += " (frameTime=" + std::to_string(ev.frameTime) + ")";
#endif
        break;
    }
    case EventType::ViewportResize: {
        const auto& ev = static_cast<const ViewportResizeEvent&>(e);
#ifndef EMSCRIPTEN
        s += fmt::format(" ({}x{})", ev.width, ev.height);
#else
        s += " (" + std::to_string(ev.width) + "x" + std::to_string(ev.height) + ")";
#endif
        break;
    }
    case EventType::AppTick: {
        const auto& ev = static_cast<const AppTickEvent&>(e);
#ifndef EMSCRIPTEN
        s += fmt::format(" (dt={:.4f})", ev.deltaTime);
#else
        s += " (dt=" + std::to_string(ev.deltaTime) + ")";
#endif
        break;
    }
    case EventType::AppUpdate: {
        const auto& ev = static_cast<const AppUpdateEvent&>(e);
#ifndef EMSCRIPTEN
        s += fmt::format(" (dt={:.4f})", ev.deltaTime);
#else
        s += " (dt=" + std::to_string(ev.deltaTime) + ")";
#endif
        break;
    }
    default:
        break;
    }

    return s;
}

#ifndef EMSCRIPTEN
} // namespace ct::events

template <>
struct fmt::formatter<ct::events::EventBase> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const ct::events::EventBase& e, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "{}", ct::events::ToString(e));
    }
};

namespace ct::events {
#endif

struct EventView {
    const EventBase& e;
};

inline EventView View(const EventBase& e) noexcept { return EventView{e}; }

} // namespace ct::events

#ifndef EMSCRIPTEN
template <>
struct fmt::formatter<ct::events::EventView> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const ct::events::EventView& v, FormatContext& ctx) const {
        return fmt::format_to(ctx.out(), "{}", ct::events::ToString(v.e));
    }
};
#endif
