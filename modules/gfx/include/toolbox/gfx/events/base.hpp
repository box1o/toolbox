#pragma once
#include "category.hpp"
#include "type.hpp"
#include <string_view>

namespace ct::events {

template <typename T>
concept Event = requires(const T t) {
    { T::GetStaticType() } -> std::same_as<EventType>;
    { t.GetEventType() } -> std::same_as<EventType>;
    { t.GetCategoryFlags() } -> std::same_as<u16>;
    { t.GetName() } -> std::convertible_to<std::string_view>;
};

struct EventBase {
    bool handled{false};
    f64 timestamp{0.0};

    virtual ~EventBase() = default;
    virtual EventType GetEventType() const = 0;
    virtual u16 GetCategoryFlags() const = 0;
    virtual std::string_view GetName() const = 0;

    [[nodiscard]] bool IsInCategory(EventCategory category) const {
        return GetCategoryFlags() & category;
    }
};

#define EVENT_TYPE(type)                                                                           \
    static ::ct::events::EventType GetStaticType() { return ::ct::events::EventType::type; }       \
    ::ct::events::EventType GetEventType() const override { return GetStaticType(); }              \
    std::string_view GetName() const override { return #type; }

#define EVENT_CATEGORY(category)                                                                   \
    u16 GetCategoryFlags() const override { return static_cast<u16>(category); }

} // namespace ct::events
