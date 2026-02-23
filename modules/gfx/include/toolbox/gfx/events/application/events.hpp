#pragma once
#include "../base.hpp"

namespace ct::events {

struct AppTickEvent : EventBase {
    f32 deltaTime;

    explicit AppTickEvent(f32 dt) : deltaTime(dt) {}

    EVENT_TYPE(AppTick)
    EVENT_CATEGORY(EventCategory::Application)
};

struct AppUpdateEvent : EventBase {
    f32 deltaTime;

    explicit AppUpdateEvent(f32 dt) : deltaTime(dt) {}

    EVENT_TYPE(AppUpdate)
    EVENT_CATEGORY(EventCategory::Application)
};

struct AppRenderEvent : EventBase {
    AppRenderEvent() = default;

    EVENT_TYPE(AppRender)
    EVENT_CATEGORY(EventCategory::Application)
};

struct AppShutdownEvent : EventBase {
    AppShutdownEvent() = default;

    EVENT_TYPE(AppShutdown)
    EVENT_CATEGORY(EventCategory::Application)
};

struct AppSuspendEvent : EventBase {
    AppSuspendEvent() = default;

    EVENT_TYPE(AppSuspend)
    EVENT_CATEGORY(EventCategory::Application)
};

struct AppResumeEvent : EventBase {
    AppResumeEvent() = default;

    EVENT_TYPE(AppResume)
    EVENT_CATEGORY(EventCategory::Application)
};

} // namespace ct::events
