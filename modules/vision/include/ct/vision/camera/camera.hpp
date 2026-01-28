#pragma once

#include "ct/math/detail/arithmetic.hpp"
#include "ct/math/math.hpp"



namespace ct::vision {

struct Camera {
    mat3f intrinsics = mat3f(
        layout::colm,
        800.0f,   0.0f, 320.0f,
          0.0f, 800.0f, 240.0f,
          0.0f,   0.0f,   1.0f
    );
    mat4f extrinsics{mat4f::identity()};
    vec<5, float> distortion = vec<5, float>(0.0f);

    Camera() = default;
    Camera(const mat3f& intrinsics, const mat4f& extrinsics)
        : intrinsics(intrinsics), extrinsics(extrinsics) {}
};

} // namespace ct::vision
