#pragma once
#include "constants.hpp"
#include <toolbox/math/math.hpp>

namespace ct::gfx {

struct alignas(16) GPUFrameData {
    mat4f view;
    mat4f projection;
    mat4f viewProjection;
    mat4f invView;
    mat4f invProjection;
    mat4f invViewProjection;
    vec4f cameraPosition;
    vec4f cameraDirection;
    vec4f viewport;
    vec4f time;
    vec4f nearFar;
};

struct alignas(16) GPUPointLight {
    vec4f positionRadius;
    vec4f colorIntensity;
};

struct alignas(16) GPUSpotLight {
    vec4f positionRange;
    vec4f directionInnerCone;
    vec4f colorOuterCone;
    vec4f params;
};

struct alignas(16) GPULightData {
    vec4f sunDirection;
    vec4f sunColorIntensity;
    vec4f ambientColorIntensity;
    u32 pointLightCount;
    u32 spotLightCount;
    u32 pad[2];
    GPUPointLight pointLights[MAX_POINT_LIGHTS];
    GPUSpotLight spotLights[MAX_SPOT_LIGHTS];
};

struct alignas(16) GPUShadowData {
    mat4f cascadeViewProj[MAX_SHADOW_CASCADES];
    vec4f cascadeSplits;
    vec4f shadowParams;
    vec4f shadowMapSize;
};

struct alignas(16) GPUMaterialData {
    vec4f baseColor;
    vec4f emissive;
    f32 metallic;
    f32 roughness;
    f32 ao;
    f32 alphaCutoff;
    f32 normalScale;
    f32 emissiveStrength;
    u32 textureFlags;
    u32 pad;
};

struct alignas(16) GPUObjectData {
    mat4f model;
    mat4f normalMatrix;
    vec4f custom;
};

} // namespace ct::gfx
