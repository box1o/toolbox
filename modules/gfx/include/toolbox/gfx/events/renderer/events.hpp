#pragma once
#include "../base.hpp"

namespace ct::events {

struct FrameBeginEvent : EventBase {
    f32 deltaTime;

    explicit FrameBeginEvent(f32 dt) : deltaTime(dt) {}

    EVENT_TYPE(FrameBegin)
    EVENT_CATEGORY(EventCategory::Render)
};

struct FrameEndEvent : EventBase {
    f32 frameTime;

    explicit FrameEndEvent(f32 ft) : frameTime(ft) {}

    EVENT_TYPE(FrameEnd)
    EVENT_CATEGORY(EventCategory::Render)
};

struct RenderBeginEvent : EventBase {
    RenderBeginEvent() = default;

    EVENT_TYPE(RenderBegin)
    EVENT_CATEGORY(EventCategory::Render)
};

struct RenderEndEvent : EventBase {
    RenderEndEvent() = default;

    EVENT_TYPE(RenderEnd)
    EVENT_CATEGORY(EventCategory::Render)
};

struct ViewportResizeEvent : EventBase {
    u32 width;
    u32 height;

    ViewportResizeEvent(u32 w, u32 h) : width(w), height(h) {}

    EVENT_TYPE(ViewportResize)
    EVENT_CATEGORY(EventCategory::Render)
};

struct SwapBuffersEvent : EventBase {
    SwapBuffersEvent() = default;

    EVENT_TYPE(SwapBuffers)
    EVENT_CATEGORY(EventCategory::Render)
};

} // namespace ct::events
