#pragma once

#include "./fwd.hpp"
#include "./base.hpp"
#include "./vec2.hpp"
#include "./vec3.hpp"
#include "./vec4.hpp"
#include "../common/functions.hpp"

namespace ct {

template<std::size_t N, arithmetic T>
[[nodiscard]] constexpr vec<N, T> min(const vec<N, T>& a, const vec<N, T>& b) noexcept {
    vec<N, T> result;
    for (std::size_t i = 0; i < N; ++i) result[i] = min(a[i], b[i]);
    return result;
}

template<std::size_t N, arithmetic T>
[[nodiscard]] constexpr vec<N, T> max(const vec<N, T>& a, const vec<N, T>& b) noexcept {
    vec<N, T> result;
    for (std::size_t i = 0; i < N; ++i) result[i] = max(a[i], b[i]);
    return result;
}

template<std::size_t N, arithmetic T>
[[nodiscard]] constexpr vec<N, T> abs(const vec<N, T>& v) noexcept {
    vec<N, T> result;
    for (std::size_t i = 0; i < N; ++i) result[i] = abs(v[i]);
    return result;
}

template<std::size_t N, arithmetic T>
[[nodiscard]] constexpr vec<N, T> clamp(const vec<N, T>& v, const vec<N, T>& lo, const vec<N, T>& hi) noexcept {
    vec<N, T> result;
    for (std::size_t i = 0; i < N; ++i) result[i] = clamp(v[i], lo[i], hi[i]);
    return result;
}

template<std::size_t N, arithmetic T>
[[nodiscard]] constexpr vec<N, T> clamp(const vec<N, T>& v, T lo, T hi) noexcept {
    vec<N, T> result;
    for (std::size_t i = 0; i < N; ++i) result[i] = clamp(v[i], lo, hi);
    return result;
}

template<std::size_t N, arithmetic T>
[[nodiscard]] constexpr T length_squared(const vec<N, T>& v) noexcept {
    return v.length_squared();
}

template<std::size_t N, arithmetic T>
[[nodiscard]] T length(const vec<N, T>& v) noexcept {
    return v.length();
}

template<std::size_t N, arithmetic T>
[[nodiscard]] vec<N, T> normalize(const vec<N, T>& v) noexcept {
    return v.normalized();
}

template<std::size_t N, arithmetic T>
[[nodiscard]] constexpr T dot(const vec<N, T>& a, const vec<N, T>& b) noexcept {
    return a. dot(b);
}

template<arithmetic T>
[[nodiscard]] constexpr vec<3, T> cross(const vec<3, T>& a, const vec<3, T>& b) noexcept {
    return a.cross(b);
}

template<arithmetic T>
[[nodiscard]] constexpr T cross(const vec<2, T>& a, const vec<2, T>& b) noexcept {
    return a.cross(b);
}

template<std::size_t N, arithmetic T>
[[nodiscard]] constexpr vec<N, T> lerp(const vec<N, T>& a, const vec<N, T>& b, T t) noexcept {
    vec<N, T> result;
    for (std::size_t i = 0; i < N; ++i) result[i] = lerp(a[i], b[i], t);
    return result;
}

template<std::size_t N, floating_point T>
[[nodiscard]] constexpr vec<N, T> reflect(const vec<N, T>& v, const vec<N, T>& n) noexcept {
    return v - n * (T{2} * dot(v, n));
}

template<std::size_t N, floating_point T>
[[nodiscard]] T distance(const vec<N, T>& a, const vec<N, T>& b) noexcept {
    return length(b - a);
}

template<std::size_t N, floating_point T>
[[nodiscard]] constexpr T distance_squared(const vec<N, T>& a, const vec<N, T>& b) noexcept {
    return length_squared(b - a);
}

} // namespace cc
