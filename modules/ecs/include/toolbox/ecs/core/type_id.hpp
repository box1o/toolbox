#pragma once

#include <atomic>
#include <type_traits>

#include <toolbox/base/base.hpp>

namespace ct::ecs {

using TypeID = u64;

namespace detail {

inline TypeID NextTypeID() noexcept {
    static std::atomic<TypeID> sCounter{0};
    return sCounter++;
}

template <typename T> TypeID TypeIDImpl() noexcept {
    static const TypeID sId = NextTypeID();
    return sId;
}

} // namespace detail

template <typename T> [[nodiscard]] inline TypeID GetTypeID() noexcept {
    return detail::TypeIDImpl<std::remove_cvref_t<T>>();
}

} // namespace ct::ecs
