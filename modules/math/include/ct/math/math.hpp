#pragma once

// IWYU pragma: begin_exports
#include "detail/arithmetic.hpp"
#include "common/constants.hpp"
#include "common/functions.hpp"

#include "vec/fwd.hpp"
#include "vec/base.hpp"
#include "vec/vec2.hpp"
#include "vec/vec3.hpp"
#include "vec/vec4.hpp"
#include "vec/functions.hpp"
#include "vec/format.hpp"

#include "mat/fwd.hpp"
#include "mat/base.hpp"
#include "mat/mat3.hpp"
#include "mat/mat4.hpp"
#include "mat/functions.hpp"
#include "mat/format.hpp"

#include "quat/fwd.hpp"
#include "quat/quat.hpp"

#include "interop/op.hpp"
#include "interop/transform.hpp"

#include "types.hpp"
// IWYU pragma: end_exports

#include <type_traits>

namespace ct {

static_assert(std::is_trivially_copyable_v<vec<2, float>>);
static_assert(std::is_trivially_copyable_v<vec<3, float>>);
static_assert(std::is_trivially_copyable_v<vec<4, float>>);
static_assert(std::is_trivially_copyable_v<mat<3, 3, float>>);
static_assert(std::is_trivially_copyable_v<mat<4, 4, float>>);

} // namespace cc
