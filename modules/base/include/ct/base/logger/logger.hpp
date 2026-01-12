#pragma once

#include <string>
#include <string_view>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <fmt/core.h>

#include "ct/base/types/types.hpp"

namespace ct::log {

enum class Level {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Critical,
    Off
};



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


inline void Configure(std::string name = "toolbox" , Level level = Level::Info, const std::string& pattern = "[%^%l%$] %v") {
    detail::Logger() = spdlog::stdout_color_mt(name);
    detail::Logger()->set_level(detail::ToSpdlog(level));
    detail::Logger()->set_pattern(pattern);
};



template<typename... Args>
inline void Trace(std::string_view fmt, Args&&... args) {
    if (auto& l = detail::Logger())
        l->trace(fmt::runtime(fmt), std::forward<Args>(args)...);
}

template<typename... Args>
inline void Debug(std::string_view fmt, Args&&... args) {
    if (auto& l = detail::Logger())
        l->debug(fmt::runtime(fmt), std::forward<Args>(args)...);
}

template<typename... Args>
inline void Info(std::string_view fmt, Args&&... args) {
    if (auto& l = detail::Logger())
        l->info(fmt::runtime(fmt), std::forward<Args>(args)...);
}

template<typename... Args>
inline void Warn(std::string_view fmt, Args&&... args) {
    if (auto& l = detail::Logger())
        l->warn(fmt::runtime(fmt), std::forward<Args>(args)...);
}

template<typename... Args>
inline void Error(std::string_view fmt, Args&&... args) {
    if (auto& l = detail::Logger())
        l->error(fmt::runtime(fmt), std::forward<Args>(args)...);
}

template<typename... Args>
inline void Critical(std::string_view fmt, Args&&... args) {
    if (auto& l = detail::Logger())
        l->critical(fmt::runtime(fmt), std::forward<Args>(args)...);
}


}






