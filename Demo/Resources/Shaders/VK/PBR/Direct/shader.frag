#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "../common.glsl"

// Vertex Input

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;

layout(location = 4) in vec3 inCameraPos;
layout(location = 5) in vec3 inFragPosInCameraViewSpace;

layout(location = 6) in mat3 inTangentMatrix;

// Motion Vectors
layout(location = 9) in vec4 inCurrentCameraPosition;
layout(location = 10) in vec4 inPreviousCameraPosition;


// Color Outputs

// PBR Scene
layout (location = 0) out vec4 outColor;
// Motion Vectors
layout (location = 1) out vec4 outMotion;
// Normal Vectors
layout (location = 2) out vec4 outNormals;


// Uniforms
// Ocupados en el vertex shader:
// 0 - 0: Cámara en el frame actual.
// 0 - 1: Cámara en el frame anterior.
// 0 - 2: resolución.

layout (set = 0, binding = 3) uniform DirLightShadowMat {
    mat4[4] matrix;
    vec4 splits;
} dirLightShadowMat;

layout (set = 0, binding = 4) uniform DirLight {
    vec4 directionAndIntensity;
    vec4 color;
} dirLight;

layout (set = 0, binding = 5) uniform samplerCube irradianceMap;
layout (set = 0, binding = 6) uniform sampler2DArray dirLightShadowMap;
layout (set = 0, binding = 7) uniform samplerCube specularMap;
layout (set = 0, binding = 8) uniform sampler2D specularLut;

layout (set = 1, binding = 0) uniform sampler2D albedoTexture;
layout (set = 1, binding = 1) uniform sampler2D normalTexture;


// Push constant
layout (push_constant) uniform Model {
    mat4 modelMatrix;
    mat4 transposedInverseModelMatrix;
    vec4 materialInfos;
} pushConstants;


void main() {
    const mat3 tangentMatrix = inTangentMatrix;

    vec3 normal = texture(normalTexture, inTexCoords).xyz;
    normal = normal * 2.0 - 1.0;
    normal = normalize(tangentMatrix * normal);

    const vec3 view = normalize(inCameraPos - inPosition);
    const vec3 reflectVec = reflect(-view, normal);

    const float metallicFactor = clamp(pushConstants.materialInfos.x, 0.02, 1.0); // TODO: texture
    const float roughnessFactor = clamp(pushConstants.materialInfos.y, 0.02, 0.95); // TODO: texture

    vec3 albedo = inColor.rgb * texture(albedoTexture, inTexCoords).xyz;

    vec3 F0 = vec3(DEFAULT_F0);
    F0 = mix(F0, albedo, metallicFactor);
    
    vec3 accummulatedRadiance = vec3(0.0);

    // Directional Light
    accummulatedRadiance += 
    CalculateShadowStrength(
        inFragPosInCameraViewSpace, 
        dirLightShadowMat.matrix, 
        dirLightShadowMat.splits, 
        dirLightShadowMap, 
        inPosition,
        1.0) 
    * GetRadiance(
        F0, 
        dirLight.directionAndIntensity.xyz, 
        view, 
        normal, 
        dirLight.color.rgb * dirLight.directionAndIntensity.w, 
        albedo, 
        max(0.085, roughnessFactor), 
        metallicFactor);

    // Irradiance Map
    const vec3 F = FresnelSchlickRoughness(max(dot(normal, view), 0.0), F0, roughnessFactor);
    vec3 kSpecular = F;
    vec3 kDiffuse = 1.0 - kSpecular;
    kDiffuse *= 1.0 - metallicFactor;
    kSpecular = 1.0 - kDiffuse;

    const vec3 irradianceMap = mix(texture(irradianceMap, normal).rgb, vec3(length(texture(irradianceMap, normal).rgb)), 0.85);
    vec3 iblDiffuse = albedo * irradianceMap;

    const float maxSpecularLod = 5 - 1;
    const vec3 prefilteredSpecularColor = textureLod(specularMap, reflectVec, roughnessFactor * maxSpecularLod).rgb;
    const vec2 lut = textureLod(specularLut, vec2(max(dot(normal, view), roughnessFactor)), 0).rg;
    const vec3 specular = prefilteredSpecularColor * (F * lut.x + lut.y);

// #define NATURAL
#define CUSTOM

#ifdef NATURAL
    const float ambientRatio = 1.0;
    const float radianceRatio = 1.0;
    const float specularRatio = 1.0;
#elif defined(CUSTOM)
    const float ambientRatio = 0.7;
    const float radianceRatio = 1.2;
    const float specularRatio = 0.8;
#else
    const float ambientRatio = 0.65;
    const float radianceRatio = 1.0;
    const float specularRatio = 0.65;
#endif

    vec3 color = vec3(0);

    color += iblDiffuse * ambientRatio * kDiffuse * 0.2;
    color += accummulatedRadiance * radianceRatio;
    color += specular * specularRatio;

    // vec3 

    color = vec3(
        max(color.r, 0.001),
        max(color.g, 0.001),
        max(color.b, 0.001)
    );

    // color *= GetShadowmapCascade(inFragPosInCameraViewSpace, dirLightShadowMat.splits, dirLightShadowMat.matrix, inPosition);

    // color = vec3(lut.y);
    // color = normal;

    outColor = vec4(color, 1.0);

    // Motion Vectors
    const vec2 cameraSpacePreviousPosition = (inPreviousCameraPosition.xy / inPreviousCameraPosition.w) * 0.5 + 0.5;
    const vec2 cameraSpaceCurrentPosition = (inCurrentCameraPosition.xy / inCurrentCameraPosition.w) * 0.5 + 0.5;

    vec2 diff = cameraSpacePreviousPosition - cameraSpaceCurrentPosition;
    diff.y *= -1.0;

    outMotion = vec4(diff, 0.0, 1.0);
    outNormals = vec4(normal.xyz * 0.5 + 0.5, 1.0);
}
