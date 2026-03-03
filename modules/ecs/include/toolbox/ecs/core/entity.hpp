#pragma once

#include <limits>
#include <toolbox/base/base.hpp>

namespace ct::ecs {

using EntityIndex = ct::u32;
using EntityVersion = ct::u32;

inline constexpr EntityIndex kNullIndex = std::numeric_limits<EntityIndex>::max();

struct Entity {
    EntityIndex index{kNullIndex};
    EntityVersion version{0};

    [[nodiscard]] constexpr bool IsNull() const noexcept {
        return index == kNullIndex && version == 0;
    }

    [[nodiscard]] constexpr explicit operator bool() const noexcept { return !IsNull(); }

    [[nodiscard]] friend constexpr bool operator==(Entity a, Entity b) noexcept = default;
};

inline constexpr Entity NullEntity{kNullIndex, 0};

} // namespace ct::ecs
