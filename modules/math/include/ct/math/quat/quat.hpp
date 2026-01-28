#pragma once

#include "../detail/arithmetic.hpp"
#include "../common/functions.hpp"
#include "../vec/base.hpp"
#include "../mat/mat3.hpp"          // IWYU pragma: keep
#include "../mat/mat4.hpp"          // IWYU pragma: keep

#include <type_traits>

namespace ct {

template<arithmetic T>
struct quat {
    using value_type = T;

    T x{0};
    T y{0};
    T z{0};
    T w{1};

    constexpr quat() noexcept = default;

    constexpr quat(T x_, T y_, T z_, T w_) noexcept
        : x(x_), y(y_), z(z_), w(w_) {}

    explicit constexpr quat(T s) noexcept
        : x(s), y(s), z(s), w(s) {}

    constexpr quat(const quat&) noexcept = default;
    constexpr quat& operator=(const quat&) noexcept = default;

    [[nodiscard]] static constexpr quat identity() noexcept {
        return quat(T{0}, T{0}, T{0}, T{1});
    }

    [[nodiscard]] static quat from_axis_angle(const vec<3, T>& axis, T angle) noexcept {
        const vec<3, T> n = axis.normalized();
        const T half = angle / T{2};
        const T s = sin(half);
        const T c = cos(half);
        return quat(n[0] * s, n[1] * s, n[2] * s, c);
    }

    [[nodiscard]] T length() const noexcept {
        return sqrt(x * x + y * y + z * z + w * w);
    }

    [[nodiscard]] constexpr T length_squared() const noexcept {
        return x * x + y * y + z * z + w * w;
    }

    [[nodiscard]] quat normalized() const noexcept {
        const T len = length();
        if (len <= epsilon<T>) {
            return identity();
        }
        const T inv = T{1} / len;
        return quat(x * inv, y * inv, z * inv, w * inv);
    }

    constexpr quat& normalize() noexcept {
        const T len = length();
        if (len <= epsilon<T>) {
            *this = identity();
            return *this;
        }
        const T inv = T{1} / len;
        x *= inv;
        y *= inv;
        z *= inv;
        w *= inv;
        return *this;
    }

    [[nodiscard]] static constexpr T dot(const quat& a, const quat& b) noexcept {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    [[nodiscard]] constexpr quat conjugate() const noexcept {
        return quat(-x, -y, -z, w);
    }

    [[nodiscard]] quat inverse() const noexcept {
        const T lsq = length_squared();
        if (lsq <= epsilon<T>) {
            return identity();
        }
        const T inv = T{1} / lsq;
        return quat(-x * inv, -y * inv, -z * inv, w * inv);
    }

    [[nodiscard]] constexpr quat operator-() const noexcept {
        return quat(-x, -y, -z, -w);
    }

    constexpr quat& operator+=(const quat& rhs) noexcept {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }

    constexpr quat& operator-=(const quat& rhs) noexcept {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }

    constexpr quat& operator*=(T s) noexcept {
        x *= s;
        y *= s;
        z *= s;
        w *= s;
        return *this;
    }

    constexpr quat& operator/=(T s) noexcept {
        x /= s;
        y /= s;
        z /= s;
        w /= s;
        return *this;
    }

    [[nodiscard]] friend constexpr quat operator+(quat a, const quat& b) noexcept {
        a += b;
        return a;
    }

    [[nodiscard]] friend constexpr quat operator-(quat a, const quat& b) noexcept {
        a -= b;
        return a;
    }

    [[nodiscard]] friend constexpr quat operator*(quat a, T s) noexcept {
        a *= s;
        return a;
    }

    [[nodiscard]] friend constexpr quat operator*(T s, quat a) noexcept {
        a *= s;
        return a;
    }

    [[nodiscard]] friend constexpr quat operator/(quat a, T s) noexcept {
        a /= s;
        return a;
    }

    [[nodiscard]] friend constexpr quat operator*(const quat& a, const quat& b) noexcept {
        return quat(
            a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
            a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
            a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w,
            a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z);
    }

    [[nodiscard]] vec<3, T> rotate(const vec<3, T>& v) const noexcept {
        const quat qv(v[0], v[1], v[2], T{0});
        const quat r = (*this) * qv * this->inverse();
        return vec<3, T>(r.x, r.y, r.z);
    }

    [[nodiscard]] mat<3, 3, T> to_mat3() const noexcept {
        const quat q = normalized();
        const T xx = q.x * q.x;
        const T yy = q.y * q.y;
        const T zz = q.z * q.z;
        const T xy = q.x * q.y;
        const T xz = q.x * q.z;
        const T yz = q.y * q.z;
        const T wx = q.w * q.x;
        const T wy = q.w * q.y;
        const T wz = q.w * q.z;

        return mat<3, 3, T>(layout::rowm,
                            T{1} - T{2} * (yy + zz), T{2} * (xy - wz),       T{2} * (xz + wy),
                            T{2} * (xy + wz),       T{1} - T{2} * (xx + zz), T{2} * (yz - wx),
                            T{2} * (xz - wy),       T{2} * (yz + wx),       T{1} - T{2} * (xx + yy));
    }

    [[nodiscard]] mat<4, 4, T> to_mat4() const noexcept {
        const mat<3, 3, T> m3 = to_mat3();
        return mat<4, 4, T>(layout::rowm,
                            m3(0, 0), m3(0, 1), m3(0, 2), T{0},
                            m3(1, 0), m3(1, 1), m3(1, 2), T{0},
                            m3(2, 0), m3(2, 1), m3(2, 2), T{0},
                            T{0},     T{0},     T{0},     T{1});
    }
};

template<arithmetic T>
[[nodiscard]] constexpr bool operator==(const quat<T>& a, const quat<T>& b) noexcept {
    if constexpr (floating_point<T>) {
        return approx_equal(a.x, b.x) &&
               approx_equal(a.y, b.y) &&
               approx_equal(a.z, b.z) &&
               approx_equal(a.w, b.w);
    }
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

template<arithmetic T>
[[nodiscard]] constexpr bool operator!=(const quat<T>& a, const quat<T>& b) noexcept {
    return !(a == b);
}

static_assert(std::is_trivially_copyable_v<quat<float>>);
static_assert(std::is_trivially_copyable_v<quat<double>>);

} // namespace cc
