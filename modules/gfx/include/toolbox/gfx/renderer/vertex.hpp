#pragma once

#include <toolbox/base/base.hpp>
#include <toolbox/math/math.hpp>

namespace ct::gfx {

struct StaticVertex {
    vec3f position;
    vec3f normal;
    vec2f uv;
    vec4f tangent;

    static constexpr u32 Stride() { return sizeof(StaticVertex); }
};

struct SkinnedVertex {
    vec3f position;
    vec3f normal;
    vec2f uv;
    vec4f tangent;
    vec4i boneIds;
    vec4f boneWeights;

    static constexpr u32 Stride() { return sizeof(SkinnedVertex); }
};

struct InstanceData {
    mat4f model;
    mat4f normalMatrix;
    vec4f color;
    vec4f custom;

    static constexpr u32 Stride() { return sizeof(InstanceData); }
};

} // namespace ct::gfx
