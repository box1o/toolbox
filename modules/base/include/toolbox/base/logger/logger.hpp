#pragma once

#include <string>
#include <string_view>

#include "toolbox/base/types/types.hpp"

#ifdef EMSCRIPTEN
#include <format>
#include <print>
#else
#include <fmt/core.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#endif

namespace ct::log {

enum class Level { Trace, Debug, Info, Warn, Error, Critical, Off };

#ifndef EMSCRIPTEN

namespace detail {

inline ref<spdlog::logger>& Logger() noexcept {
    static ref<spdlog::logger> logger;
    return logger;
}

inline spdlog::level::level_enum ToSpdlog(Level level) noexcept {
    switch (level) {
    case Level::Trace:    return spdlog::level::trace;
    case Level::Debug:    return spdlog::level::debug;
    case Level::Info:     return spdlog::level::info;
    case Level::Warn:     return spdlog::level::warn;
    case Level::Error:    return spdlog::level::err;
    case Level::Critical: return spdlog::level::critical;
    case Level::Off:      return spdlog::level::off;
    default:              return spdlog::level::info;
    }
}

} // namespace detail

inline void Configure(std::string name = "toolbox", Level level = Level::Info,
    const std::string& pattern = "[%^%l%$] %v") {
    detail::Logger() = spdlog::stdout_color_mt(name);
    detail::Logger()->set_level(detail::ToSpdlog(level));
    detail::Logger()->set_pattern(pattern);
}

template <typename... Args> inline void Trace(std::string_view fmt, Args&&... args) {
    if (auto& l = detail::Logger()) l->trace(fmt::runtime(fmt), std::forward<Args>(args)...);
}
template <typename... Args> inline void Debug(std::string_view fmt, Args&&... args) {
    if (auto& l = detail::Logger()) l->debug(fmt::runtime(fmt), std::forward<Args>(args)...);
}
template <typename... Args> inline void Info(std::string_view fmt, Args&&... args) {
    if (auto& l = detail::Logger()) l->info(fmt::runtime(fmt), std::forward<Args>(args)...);
}
template <typename... Args> inline void Warn(std::string_view fmt, Args&&... args) {
    if (auto& l = detail::Logger()) l->warn(fmt::runtime(fmt), std::forward<Args>(args)...);
}
template <typename... Args> inline void Error(std::string_view fmt, Args&&... args) {
    if (auto& l = detail::Logger()) l->error(fmt::runtime(fmt), std::forward<Args>(args)...);
}
template <typename... Args> inline void Critical(std::string_view fmt, Args&&... args) {
    if (auto& l = detail::Logger()) l->critical(fmt::runtime(fmt), std::forward<Args>(args)...);
}

#else

inline void Configure(std::string = "toolbox", Level = Level::Info, const std::string& = {}) {}

namespace detail {

inline constexpr std::string_view Prefix(Level level) noexcept {
    switch (level) {
    case Level::Trace:    return "[TRACE] ";
    case Level::Debug:    return "[DEBUG] ";
    case Level::Info:     return "[INFO ] ";
    case Level::Warn:     return "[WARN ] ";
    case Level::Error:    return "[ERROR] ";
    case Level::Critical: return "[FATAL] ";
    default:              return "";
    }
}

//NOTE: std::vformat takes runtime string_view, std::format requires consteval
template <typename... Args> inline void Print(Level level, std::string_view fmt, Args&&... args) {
    auto formatted = std::vformat(fmt, std::make_format_args(args...));
    std::println("{}{}", Prefix(level), formatted);
}

} // namespace detail

template <typename... Args> inline void Trace(std::string_view fmt, Args&&... args) {
    detail::Print(Level::Trace, fmt, args...);
}
template <typename... Args> inline void Debug(std::string_view fmt, Args&&... args) {
    detail::Print(Level::Debug, fmt, args...);
}
template <typename... Args> inline void Info(std::string_view fmt, Args&&... args) {
    detail::Print(Level::Info, fmt, args...);
}
template <typename... Args> inline void Warn(std::string_view fmt, Args&&... args) {
    detail::Print(Level::Warn, fmt, args...);
}
template <typename... Args> inline void Error(std::string_view fmt, Args&&... args) {
    detail::Print(Level::Error, fmt, args...);
}
template <typename... Args> inline void Critical(std::string_view fmt, Args&&... args) {
    detail::Print(Level::Critical, fmt, args...);
}

#endif

} // namespace ct::log
