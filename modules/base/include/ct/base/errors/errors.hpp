#pragma once

#include <string>
#include <string_view>
#include <source_location>
#include "ct/base/types/types.hpp"

namespace ct {

enum class ErrorType : u8 {
    CORE = 0,
    FILE_SYSTEM,
    NETWORK,
    PARSE,
    VALIDATION,
    GRAPHICS
};

enum class ErrorCode : u16 {
    SUCCESS = 0,

    FILE_NOT_FOUND = 100,
    FILE_ACCESS_DENIED,
    FILE_READ_ERROR,
    FILE_WRITE_ERROR,
    FILE_EOF,

    NETWORK_CONNECTION_FAILED = 200,
    NETWORK_CONNECTION_CLOSED,
    NETWORK_TIMEOUT,
    NETWORK_HOST_UNREACHABLE,
    NETWORK_SEND_FAILED,
    NETWORK_RECEIVE_FAILED,

    PARSE_INVALID_FORMAT = 300,
    PARSE_UNEXPECTED_TOKEN,
    PARSE_MISSING_FIELD,
    PARSE_TYPE_MISMATCH,

    VALIDATION_NULL_VALUE = 400,
    VALIDATION_OUT_OF_RANGE,
    VALIDATION_INVALID_STATE,

    GRAPHICS_INIT_FAILED = 500,
    GRAPHICS_DEVICE_LOST,
    GRAPHICS_INVALID_FORMAT,
    GRAPHICS_FRAMEBUFFER_INCOMPLETE,
    GRAPHICS_SHADER_COMPILATION_FAILED,
    GRAPHICS_TEXTURE_CREATION_FAILED,
    GRAPHICS_BUFFER_CREATION_FAILED,
    GRAPHICS_UNSUPPORTED_API,

    UNKNOWN_ERROR = 999
};

class Error {
public:
    constexpr Error(
        ErrorCode code,
        std::string_view msg,
        std::source_location loc = std::source_location::current()) noexcept
        : mCode(code)
        , mMessage(msg)
        , mLocation(loc) {}

    [[nodiscard]] constexpr ErrorCode Code() const noexcept {
        return mCode;
    }

    [[nodiscard]] constexpr ErrorType Type() const noexcept {
        const u16 code_val = static_cast<u16>(mCode);

        if (code_val >= 100 && code_val < 200) return ErrorType::FILE_SYSTEM;
        if (code_val >= 200 && code_val < 300) return ErrorType::NETWORK;
        if (code_val >= 300 && code_val < 400) return ErrorType::PARSE;
        if (code_val >= 400 && code_val < 500) return ErrorType::VALIDATION;
        if (code_val >= 500 && code_val < 600) return ErrorType::GRAPHICS;
        return ErrorType::CORE;
    }

    [[nodiscard]] constexpr std::string_view Message() const noexcept {
        return mMessage;
    }

    [[nodiscard]] constexpr const std::source_location& Location() const noexcept {
        return mLocation;
    }

    void Log() const;

private:
    ErrorCode mCode;
    std::string mMessage;
    std::source_location mLocation;
};

} // namespace ct
