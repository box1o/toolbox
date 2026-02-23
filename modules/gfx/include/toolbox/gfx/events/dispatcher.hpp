#pragma once
#include "base.hpp"
#include <type_traits>

namespace ct::events {

template <typename E> class EventDispatcher {
public:
    explicit EventDispatcher(E& event) : event_(event) {}

    template <typename T, typename Fn>
        requires Event<T> && std::invocable<Fn, const T&>
    bool Dispatch(Fn&& fn) {
        if (event_.GetEventType() == T::GetStaticType() && !event_.handled) {
            if constexpr (std::is_same_v<std::invoke_result_t<Fn, const T&>, bool>) {
                event_.handled = fn(static_cast<const T&>(event_));
            } else {
                fn(static_cast<const T&>(event_));
                event_.handled = false;
            }
            return true;
        }
        return false;
    }

private:
    E& event_;
};

} // namespace ct::events
