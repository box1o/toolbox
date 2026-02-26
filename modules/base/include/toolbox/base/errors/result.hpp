#pragma once

#include "errors.hpp"
#include <expected>
#include <source_location>
#include <type_traits>
#include <utility>

#ifdef DEBUG
#include "toolbox/base/logger/logger.hpp"
#include <cstdlib>
#endif

namespace ct {

template <typename T> using result = std::expected<T, Error>;

namespace detail {

inline void unwrap(result<void>&& r) noexcept {
#ifdef DEBUG
    if (!r) {
        log::Critical("Attempted to unwrap an error result: {}", r.error().Message());
        std::abort();
    }
#endif
    // nothing to return
}

template <typename T> constexpr T&& unwrap(result<T>&& r) noexcept {
#ifdef DEBUG
    if (!r) {
        log::Critical("Attempted to unwrap an error result: {}", r.error().Message());
        std::abort();
    }
#endif
    return std::move(*r);
}

} // namespace detail

template <typename T>
    requires(!std::same_as<std::decay_t<T>, std::unexpected<Error>>)

[[nodiscard]] constexpr auto ok(T&& value) noexcept -> result<std::decay_t<T>> {
    return result<std::decay_t<T>>(std::forward<T>(value));
}

[[nodiscard]] constexpr auto ok() noexcept -> result<void> { return result<void>(); }

[[nodiscard]] inline Error make_error(ErrorCode code, std::string_view msg = {},
    std::source_location loc = std::source_location::current()) noexcept {
    return Error(code, msg, loc);
}

[[nodiscard]] inline auto err(ErrorCode code, std::string_view msg,
    std::source_location loc = std::source_location::current()) noexcept -> std::unexpected<Error> {
    return std::unexpected(make_error(code, msg, loc));
}

[[nodiscard]] inline auto err(ErrorCode code,
    std::source_location loc = std::source_location::current()) noexcept -> std::unexpected<Error> {
    return std::unexpected(make_error(code, {}, loc));
}

[[nodiscard]] inline auto err(const Error& e) noexcept -> std::unexpected<Error> {
    return std::unexpected(e);
}

#define TRY_ASSIGN(lhs, expr)                                                                      \
    auto _try_result_##lhs = (expr);                                                               \
    if (!_try_result_##lhs) return _try_result_##lhs.error();                                      \
    lhs = ::ct::detail::unwrap(std::move(_try_result_##lhs))

#define TRY_VOID(expr)                                                                             \
    do {                                                                                           \
        auto _try_result = (expr);                                                                 \
        if (!_try_result) std::abort();                                                            \
    } while (0)

#define TRY(expr)                                                                                  \
    ({                                                                                             \
        auto _try_result = (expr);                                                                 \
        if (!_try_result) {                                                                        \
            const char* _err_file = __FILE__;                                                      \
            int _err_line = __LINE__;                                                              \
            std::string _err_msg{_try_result.error().Message()};                                   \
            log::Critical("TRY failed at {}:{}: {}", _err_file, _err_line, _err_msg);              \
            std::abort();                                                                          \
        }                                                                                          \
        ::ct::detail::unwrap(std::move(_try_result));                                              \
    })

#define TRY_RETURN(expr)                                                                           \
    ({                                                                                             \
        auto _try_result = (expr);                                                                 \
        if (!_try_result) {                                                                        \
            const char* _err_file = __FILE__;                                                      \
            int _err_line = __LINE__;                                                              \
            std::string _err_msg{_try_result.error().Message()};                                   \
            log::Critical("TRY_RETURN failed at {}:{}: {}", _err_file, _err_line, _err_msg);       \
            return err(_try_result.error());                                                       \
        }                                                                                          \
        ::ct::detail::unwrap(std::move(_try_result));                                              \
    })

} // namespace ct
