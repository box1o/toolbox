#pragma once

#include <toolbox/base/types/types.hpp>

namespace ct::gfx {

constexpr u32 MAX_POINT_LIGHTS = 256;
constexpr u32 MAX_SPOT_LIGHTS = 64;
constexpr u32 MAX_SHADOW_CASCADES = 4;
constexpr u32 MAX_POINT_LIGHT_SHADOWS = 4;
constexpr u32 MAX_SPOT_LIGHT_SHADOWS = 8;
constexpr u32 MAX_TEXTURE_SLOTS = 16;
constexpr u32 MAX_BONES = 128;
constexpr u32 MAX_INSTANCES_PER_BATCH = 4096;
constexpr u32 MAX_DRAW_COMMANDS = 100000;

constexpr u32 SET_GLOBAL = 0;
constexpr u32 SET_MATERIAL = 1;
constexpr u32 SET_OBJECT = 2;

constexpr u32 BIND_FRAME_UBO = 0;
constexpr u32 BIND_LIGHT_UBO = 1;
constexpr u32 BIND_SHADOW_UBO = 2;
constexpr u32 BIND_SHADOW_MAPS = 3;
constexpr u32 BIND_ENV_IRRADIANCE = 4;
constexpr u32 BIND_ENV_PREFILTER = 5;
constexpr u32 BIND_ENV_BRDF = 6;
constexpr u32 BIND_OBJECT = 10; // object UBO binding used by shaders

constexpr u32 TEX_ALBEDO = 0;
constexpr u32 TEX_NORMAL = 1;
constexpr u32 TEX_METALLIC_ROUGHNESS = 2;
constexpr u32 TEX_AO = 3;
constexpr u32 TEX_EMISSIVE = 4;
constexpr u32 TEX_CUSTOM_0 = 5;
constexpr u32 TEX_CUSTOM_1 = 6;
constexpr u32 TEX_CUSTOM_2 = 7;
constexpr u32 TEX_CUSTOM_3 = 8;

} // namespace ct::gfx
