#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "../../common.glsl"

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec2 inTexCoords;
layout(location = 2) in vec2 inPos;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 1) uniform Camera {
    mat4 projection;
    mat4 view;

    vec3 cameraPos;
} camera;


// Lights & shadows
layout (set = 0, binding = 2) uniform DirLightShadowMat {
    mat4[4] matrix;
    vec4 splits;
} dirLightShadowMat;

layout (set = 0, binding = 3) uniform DirLight {
    vec4 directionAndIntensity;
    vec4 color;
} dirLight;

layout (set = 0, binding = 4) uniform samplerCube irradianceMap;
layout (set = 0, binding = 5) uniform sampler2DArray dirLightShadowMap;


layout (set = 1, binding = 0) uniform sampler2D positionTexture;
layout (set = 1, binding = 1) uniform sampler2D colorTexture;
layout (set = 1, binding = 2) uniform sampler2D normalTexture;

void main() {
    const vec4 colorInfo = texture(colorTexture, inTexCoords);
    const vec4 normalInfo = texture(normalTexture, inTexCoords);

    const vec3 worlPosition = texture(positionTexture, inTexCoords).xyz;
    const vec4 albedo = vec4(colorInfo.rgb, 1.0);
    const vec3 normal = normalInfo.xyz;

    const vec3 view = normalize(camera.cameraPos - worlPosition);
    const vec3 reflectDirection = reflect(-view, normal);
    
    const float metallicFactor = float(int(normalInfo.a)) * 0.001; // TODO: texture
    const float roughnessFactor = (normalInfo.a - float(int(normalInfo.a))) * 10; // TODO: texture
    
    vec3 F0 = vec3(DEFAULT_F0);
    F0 = mix(F0, albedo.rgb, metallicFactor);

    // Camera-Space: para el mapa de sombras
    const vec4 temp = camera.view * vec4(worlPosition, 1.0);
    const vec3 cameraSpacePosition = (temp / temp.w).xyz;

    // Directional Light
    const vec3 accummulatedRadiance = CalculateShadowStrength(cameraSpacePosition, dirLightShadowMat.matrix, dirLightShadowMat.splits, dirLightShadowMap, worlPosition) 
                    * GetRadiance(F0, dirLight.directionAndIntensity.xyz, view, normal, dirLight.color.rgb * dirLight.directionAndIntensity.w, albedo.rgb, roughnessFactor, metallicFactor);

    // Irradiance Map
    vec3 kS = FreshnelShlick(max(dot(normal, view), 0.0), F0);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallicFactor;

    const vec3 irradiance = texture(irradianceMap, normal).rgb;
    const vec3 ambient = albedo.rgb * kD * irradiance;
    
    outColor = vec4(ambient * (dirLight.directionAndIntensity.w * 0.25) + accummulatedRadiance * 1.25, colorInfo.a);
    // outColor = vec4(vec3(roughnessFactor), 1.0);
}
