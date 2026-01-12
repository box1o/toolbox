
#pragma once
#include "errors.hpp"
#include <expected>
#include <utility>

namespace ct {

template<typename T>
using result = std::expected<T, Error>;

template<typename T>
constexpr auto ok(T&& value) noexcept -> result<std::decay_t<T>> {
    return result<std::decay_t<T>>(std::forward<T>(value));
}

constexpr auto ok() noexcept -> result<void> {
    return result<void>();
}

inline auto err(ErrorCode code, std::string_view msg,
                std::source_location loc = std::source_location::current()) noexcept 
-> std::unexpected<Error> {
    return std::unexpected<Error>(Error(code, msg, loc));
}

inline auto err(ErrorCode code,
                std::source_location loc = std::source_location::current()) noexcept
-> std::unexpected<Error> {
    return std::unexpected<Error>(Error(code, "", loc));
}

inline auto err(const Error& e) noexcept -> std::unexpected<Error> {
    return std::unexpected<Error>(e);
}

} // namespace cc
