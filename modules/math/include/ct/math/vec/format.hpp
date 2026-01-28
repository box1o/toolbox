#pragma once

#include "./base.hpp"
#include <format>

namespace std {

template<std::size_t N, ct::arithmetic T>
struct formatter<ct::vec<N, T>> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    auto format(const ct::vec<N, T>& v, format_context& ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "vec{}(", N);
        for (std::size_t i = 0; i < N; ++i) {
            out = std::format_to(out, "{}", v[i]);
            if (i + 1 < N) out = std::format_to(out, ", ");
        }
        return std::format_to(out, ")");
    }
};

} // namespace std
