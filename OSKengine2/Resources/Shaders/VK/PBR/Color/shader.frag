#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "../common.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;

layout(location = 4) in vec3 inCameraPos;
layout(location = 5) in vec3 inFragPosInCameraViewSpace;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outLightBrightness;

layout (set = 0, binding = 1) uniform DirLightShadowMat {
    mat4[4] matrix;
    vec4 splits;
} dirLightShadowMat;

layout (set = 0, binding = 2) uniform DirLight {
    vec4 directionAndIntensity;
    vec4 color;
} dirLight;

layout (set = 0, binding = 3) uniform samplerCube irradianceMap;
layout (set = 0, binding = 4) uniform sampler2DArray dirLightShadowMap;

layout (set = 1, binding = 0) uniform sampler2D stexture;
// layout (set = 0, binding = 1) uniform sampler2D metallicTexture;
// layout (set = 0, binding = 2) uniform sampler2D roughnessTexture;

layout (push_constant) uniform MaterialInfo {
    // x = metallic
    // y = roughness
    layout (offset = 64) vec4 infos;
} materialInfo;

void main() {
    const vec3 normal = normalize(inNormal);
    const vec3 view = normalize(inCameraPos - inPosition);
    const vec3 reflectVec = reflect(-view, normal);

    const float metallicFactor = materialInfo.infos.x; // TODO: texture
    const float roughnessFactor = materialInfo.infos.y; // TODO: texture

    vec3 albedo = texture(stexture, inTexCoords).xyz;

    vec3 F0 = vec3(DEFAULT_F0);
    F0 = mix(F0, albedo, metallicFactor);

    vec3 accummulatedRadiance = vec3(0.0);

    // Directional Light
    accummulatedRadiance += CalculateShadow() * GetRadiance(F0, dirLight.directionAndIntensity.xyz, view, normal, dirLight.color.rgb * dirLight.directionAndIntensity.w, albedo, roughnessFactor, metallicFactor);

    // Irradiance Map
    vec3 kS = FreshnelShlick(max(dot(normal, view), 0.0), F0);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallicFactor;

    const vec3 irradiance = texture(irradianceMap, normal).rgb;
    const vec3 ambient = albedo * kD * irradiance;
    vec3 color = ambient * (dirLight.directionAndIntensity.w * 0.25) + accummulatedRadiance * 1.25;

    outColor = vec4(color, 1.0);
}
