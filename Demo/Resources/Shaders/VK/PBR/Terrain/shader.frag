#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "../common.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 projection;

    vec3 cameraPos;
} camera;

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

layout (set = 1, binding = 0) uniform sampler2D albedoTexture;

layout (push_constant) uniform Model {
    mat4 modelMatrix;
    mat4 transposedInverseModelMatrix;
    vec4 materialInfos;
} pushConstants;

void main() {
    const vec4 temp = camera.view * vec4(inPosition, 1.0);
    const vec3 fragPosInCameraViewSpace = (temp / temp.w).xyz;

    const vec3 normal = normalize(inNormal);
    const vec3 view = normalize(camera.cameraPos - inPosition);
    const vec3 reflectVec = reflect(-view, normal);

    const float metallicFactor = pushConstants.materialInfos.x; // TODO: texture
    const float roughnessFactor = pushConstants.materialInfos.y; // TODO: texture

    vec3 albedo = texture(albedoTexture, inTexCoords).xyz;

    vec3 F0 = vec3(DEFAULT_F0);
    F0 = mix(F0, albedo, metallicFactor);

    vec3 accummulatedRadiance = vec3(0.0);

    // Directional Light
    accummulatedRadiance += CalculateShadowStrength(fragPosInCameraViewSpace, dirLightShadowMat.matrix, dirLightShadowMat.splits, dirLightShadowMap, inPosition) 
        * GetRadiance(F0, dirLight.directionAndIntensity.xyz, view, normal, dirLight.color.rgb * dirLight.directionAndIntensity.w, albedo, roughnessFactor, metallicFactor);

    // Irradiance Map
    vec3 kS = FreshnelShlick(max(dot(normal, view), 0.0), F0);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - (metallicFactor * 0.2);

    const vec3 irradiance = texture(irradianceMap, normal).rgb;
    const vec3 ambient = albedo * kD * irradiance;
    vec3 color = ambient * (dirLight.directionAndIntensity.w * 0.25) + accummulatedRadiance * 1.25;

    outColor = vec4(color, 1.0);
    outColor = vec4(1.0);
}
