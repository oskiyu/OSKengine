#version 460
#extension GL_ARB_separate_shader_objects : enable

#include "../../Common/pbr.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;

layout(location = 4) in vec3 inCameraPos;
layout(location = 5) in vec3 inFragPosInCameraViewSpace;

layout (location = 0) out vec4 outColor;

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

vec3 GetRadiance(vec3 F0, vec3 direction, vec3 view, vec3 normal, vec3 lightColor, vec3 albedoColor, float roughnessFactor, float metallicFactor);
float CalculateShadow();
vec3 GetShadowmapCascade();
vec3 GetShadowCoordinates();

void main() {
    const vec3 normal = normalize(inNormal);
    const vec3 view = normalize(inCameraPos - inPosition);
    const vec3 reflectVec = reflect(-view, normal);

    const float metallicFactor = materialInfo.infos.x; // TODO: texture
    const float roughnessFactor = materialInfo.infos.y; // TODO: texture

    const vec3 albedo = texture(stexture, inTexCoords).xyz;

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
    vec3 color = ambient * 2 + accummulatedRadiance;

    vec3 scolor = color / (color + vec3(1.0));
    scolor = pow(scolor, vec3(1.0/2.2));

    color = mix(color, scolor, 0.3);

    outColor = vec4(color, 1.0);
    // outColor = vec4(mix(color, GetShadowmapCascade(), 0.3), 1.0);
}

vec3 GetRadiance(vec3 F0, vec3 direction, vec3 view, vec3 normal, vec3 lightColor, vec3 albedoColor, float roughnessFactor, float metallicFactor) {
    vec3 L = -direction;
    vec3 H = normalize(view + L);
    vec3 radiance = lightColor;

    // Cook-Torrance
    float NDF = DistributionGGX(normal, H, roughnessFactor);
    float G = GeometrySmith(normal, view, L, roughnessFactor);
    vec3 F = FreshnelShlick(max(dot(H, view), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallicFactor;

    vec3 num = NDF * G * F;
    float denom = 4.0 * max(dot(normal, view), 0.0) * max(dot(normal, L), 0.0) + 0.0001;
    vec3 specular = num / denom;

    return (kD * albedoColor / PI + specular) * radiance * max(dot(normal, L), 0.0);
}

float CalculateShadow() {
    // Cascaded Shadow Map
    const float viewDepth = inFragPosInCameraViewSpace.z;
    int shadowMapIndex = 0;
    for(int i = 0; i < 4 - 1; i++) {
		if(viewDepth < dirLightShadowMat.splits[i]) {	
			shadowMapIndex = i + 1;
		}
	}

    vec4 fragPosInLightSpace = dirLightShadowMat.matrix[shadowMapIndex] * vec4(inPosition, 1.0);
   
    vec4 projCoords = (fragPosInLightSpace / fragPosInLightSpace.w);
    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    projCoords.y = -projCoords.y;

    const float currentDepth = projCoords.z - 0.01;

    float accumulatedShadow = 0.0;
    const vec2 texelSize = 1.0 / textureSize(dirLightShadowMap, 0).xy;

    for (int x = -1; x < 1; x++) {
        for (int y = -1; y < 1; y++) {
            const vec2 finalCoords = projCoords.xy + vec2(x, y) * texelSize;
            if (finalCoords.x > 1.0 || finalCoords.x < -1.0 || finalCoords.y > 1.0 || finalCoords.y < -1.0) {
                continue;
            }

            float pointDepth = texture(dirLightShadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, shadowMapIndex)).r;
            accumulatedShadow += (currentDepth < pointDepth) ? 1.0 : 0.0;
        }
    }

    return accumulatedShadow / 9.0;
}

vec3 GetShadowmapCascade() {
    const float viewDepth = inFragPosInCameraViewSpace.z;
    int shadowMapIndex = 0;
    for(int i = 0; i < 4 - 1; i++) {
		if(viewDepth < dirLightShadowMat.splits[i]) {	
			shadowMapIndex = i + 1;
		}
	}

    switch (shadowMapIndex) {
        case 0: return vec3(0.0, 0.0, 0.0);
        case 1: return vec3(1.0, 0.0, 0.0);
        case 2: return vec3(0.0, 1.0, 0.0);
        case 3: return vec3(0.0, 0.0, 1.0);

        default: return vec3(1.0, 1.0, 1.0);
    }
    
}

vec3 GetShadowCoordinates() {
      // Cascaded Shadow Map
    const float viewDepth = inFragPosInCameraViewSpace.z;
    int shadowMapIndex = 0;
    for(int i = 0; i < 4 - 1; i++) {
		if(viewDepth < dirLightShadowMat.splits[i]) {	
			shadowMapIndex = i + 1;
		}
	}

    const vec4 fragPosInLightSpace = dirLightShadowMat.matrix[shadowMapIndex] * vec4(inPosition, 1.0);
    vec4 projCoords = (fragPosInLightSpace / fragPosInLightSpace.w);
    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    projCoords.y = -projCoords.y;

    return vec3(projCoords.xy, 1.0);
}
