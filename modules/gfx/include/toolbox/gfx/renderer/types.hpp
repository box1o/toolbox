#pragma once
#include <cfloat>

#include <toolbox/base/base.hpp>
#include <toolbox/math/math.hpp>

namespace ct::gfx {

enum class RenderLayer : u8 {
    Background = 0,
    Opaque = 1,
    AlphaTest = 2,
    Transparent = 3,
    Overlay = 4,
    Count
};

enum class BlendMode : u8 { Opaque, AlphaTest, AlphaBlend, Additive, Multiply };
enum class ShadingModel : u8 { Unlit, Lit, PBR, Subsurface, ClearCoat, Custom };

enum class RenderFlags : u32 {
    None = 0,
    CastShadow = 1 << 0,
    ReceiveShadow = 1 << 1,
    Static = 1 << 2,
    Dynamic = 1 << 3,
    TwoSided = 1 << 4,
    Instanced = 1 << 5,
    Default = CastShadow | ReceiveShadow | Dynamic
};

inline RenderFlags operator|(RenderFlags a, RenderFlags b) {
    return static_cast<RenderFlags>(static_cast<u32>(a) | static_cast<u32>(b));
}

inline RenderFlags operator&(RenderFlags a, RenderFlags b) {
    return static_cast<RenderFlags>(static_cast<u32>(a) & static_cast<u32>(b));
}

inline bool HasFlag(RenderFlags flags, RenderFlags flag) {
    return (static_cast<u32>(flags) & static_cast<u32>(flag)) != 0;
}

struct AABB {
    vec3f min{FLT_MAX};
    vec3f max{-FLT_MAX};

    [[nodiscard]] vec3f Center() const { return (min + max) * 0.5f; }
    [[nodiscard]] vec3f Extents() const { return (max - min) * 0.5f; }

    void Expand(const vec3f& point) {
        min.x = std::min(min.x, point.x);
        min.y = std::min(min.y, point.y);
        min.z = std::min(min.z, point.z);

        max.x = std::max(max.x, point.x);
        max.y = std::max(max.y, point.y);
        max.z = std::max(max.z, point.z);
    }

    [[nodiscard]] AABB Transform(const mat4f& m) const {
        AABB result;
        vec3f corners[8] = {{min.x, min.y, min.z}, {max.x, min.y, min.z}, {min.x, max.y, min.z},
            {max.x, max.y, min.z}, {min.x, min.y, max.z}, {max.x, min.y, max.z},
            {min.x, max.y, max.z}, {max.x, max.y, max.z}};
        for (const auto& c : corners) {
            vec4f transformed = m * vec4f(c, 1.0f);
            result.Expand(transformed.xyz());
        }
        return result;
    }
};

struct Frustum {
    vec4f planes[6]{};

    [[nodiscard]] static Frustum FromViewProjection(const mat4f& vp) {
        Frustum f;
        f.planes[0] = vec4f(
            vp[0][3] + vp[0][0], vp[1][3] + vp[1][0], vp[2][3] + vp[2][0], vp[3][3] + vp[3][0]);
        f.planes[1] = vec4f(
            vp[0][3] - vp[0][0], vp[1][3] - vp[1][0], vp[2][3] - vp[2][0], vp[3][3] - vp[3][0]);
        f.planes[2] = vec4f(
            vp[0][3] + vp[0][1], vp[1][3] + vp[1][1], vp[2][3] + vp[2][1], vp[3][3] + vp[3][1]);
        f.planes[3] = vec4f(
            vp[0][3] - vp[0][1], vp[1][3] - vp[1][1], vp[2][3] - vp[2][1], vp[3][3] - vp[3][1]);
        f.planes[4] = vec4f(
            vp[0][3] + vp[0][2], vp[1][3] + vp[1][2], vp[2][3] + vp[2][2], vp[3][3] + vp[3][2]);
        f.planes[5] = vec4f(
            vp[0][3] - vp[0][2], vp[1][3] - vp[1][2], vp[2][3] - vp[2][2], vp[3][3] - vp[3][2]);

        for (auto& p : f.planes) {
            f32 len = length(p.xyz());
            p = p / len;
        }
        return f;
    }

    [[nodiscard]] bool Contains(const AABB& box) const {
        vec3f center = box.Center();
        vec3f extents = box.Extents();

        for (const auto& plane : planes) {
            vec3f normal{plane.x, plane.y, plane.z};
            f32 d = plane.w;
            f32 r = dot(extents, abs(normal));
            f32 s = dot(center, normal) + d;
            if (s + r < 0.0f) return false;
        }
        return true;
    }
};

struct RenderStats {
    u32 drawCalls = 0;
    u32 instancedDrawCalls = 0;
    u32 triangles = 0;
    u32 vertices = 0;
    u32 visibleObjects = 0;
    u32 culledObjects = 0;
    u32 batchedInstances = 0;
    u32 materialSwitches = 0;
    u32 shaderSwitches = 0;
    u32 lightCount = 0;
    f32 frameTimeMs = 0.0f;

    void Reset() { *this = {}; }
};

} // namespace ct::gfx
