#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "../../common.glsl"

layout(location = 0) in vec2 inTexCoords;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform Camera {
    mat4 projection;
    mat4 view;

    vec3 cameraPos;
} camera;


// Lights & shadows
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
layout (set = 0, binding = 5) uniform samplerCube specularMap;
layout (set = 0, binding = 6) uniform sampler2D specularLut;


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

    vec3 accummulatedRadiance = vec3(0.0);
    
    // Camera-Space: para el mapa de sombras
    const vec4 temp = camera.view * vec4(worlPosition, 1.0);
    const vec3 cameraSpacePosition = (temp / temp.w).xyz;
    
    // Directional Light
    accummulatedRadiance += CalculateShadowStrength(cameraSpacePosition, dirLightShadowMat.matrix, dirLightShadowMat.splits, dirLightShadowMap, worlPosition) 
        * GetRadiance(F0, dirLight.directionAndIntensity.xyz, view, normal, dirLight.color.rgb * dirLight.directionAndIntensity.w, albedo.rgb, roughnessFactor, metallicFactor);

    // Irradiance Map
    vec3 kS = FreshnelShlick(max(dot(normal, view), 0.0), F0);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - (metallicFactor); // * 0.2

    const vec3 irradiance = texture(irradianceMap, normal).rgb;
    const vec3 ambient = albedo.rgb * kD * irradiance;

    const float maxSpecularLod = 5 - 1;
    const vec3 F = FresnelSchlickRoughness(max(dot(normal, view), 0.0), F0, roughnessFactor);
    const vec3 prefilteredSpecularColor = textureLod(specularMap, reflectDirection, roughnessFactor * maxSpecularLod).rgb;
    const vec2 lut = textureLod(specularLut, vec2(max(dot(normal, view), roughnessFactor)), 0).rg;
    const vec3 specular = prefilteredSpecularColor * (F * lut.x + lut.y);
    
    
    vec3 color = ambient * (dirLight.directionAndIntensity.w * 0.25) 
                + accummulatedRadiance * 1.25 
                + specular * 0.45;
    
    // vec3 color = ambient * (dirLight.directionAndIntensity.w * 0.5) + accummulatedRadiance * 1.25 + specular * 0.5;
    // vec3 color = ambient + accummulatedRadiance + specular;
    
    // vec3 color = vec3(lut.y);

    color = vec3(
        max(color.r, 0.01),
        max(color.g, 0.01),
        max(color.b, 0.01)
    );

    outColor = vec4(color, colorInfo.a);
}
