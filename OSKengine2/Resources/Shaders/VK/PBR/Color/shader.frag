#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;

layout(location = 4) in vec3 inCameraPos;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 1) uniform sampler2D stexture;
layout (set = 0, binding = 4) uniform samplerCube irradianceMap;
// layout (set = 0, binding = 2) uniform sampler2D metallicTexture;
// layout (set = 0, binding = 3) uniform sampler2D roughnessTexture;

layout (set = 0, binding = 2) uniform DirLight {
    vec3 direction;
    vec4 color;
    float intensity;
} dirLight;

layout (push_constant) uniform MaterialInfo {
    // x = metallic
    // y = roughness
    layout (offset = 64) vec4 infos;
} materialInfo;

// Calcula el ratio entre la cantidad entre diffuse y specular.
//  F0: reflejo al mirar directamente a la superficie.
vec3 FreshnelShlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 0.5);
}

const float DEFAULT_F0 = 0.04;
const float PI = 3.14159265359;

float DistributionGGX(vec3 normal, vec3 h, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;

    float NdotH = max(dot(normal, h), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = NdotH2 * (a2 - 1.0) + 1.0;
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 normal, vec3 view, vec3 L, float roughness) {
    float NdotV = max(dot(normal, view), 0.0);
    float NdotL = max(dot(normal, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

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
    vec3 L = -dirLight.direction;
    vec3 H = normalize(view + L);
    vec3 radiance = dirLight.color.xyz * dirLight.intensity;

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
    accummulatedRadiance += (kD * albedo / PI + specular) * radiance * max(dot(normal, L), 0.0);

    // Irradiance Map
    kS = FreshnelShlick(max(dot(normal, view), 0.0), F0);
    kD = 1.0 - kS;
    kD *= 1.0 - metallicFactor;

    const vec3 irradiance = texture(irradianceMap, normal).rgb;
    const vec3 ambient = albedo * kD * irradiance;
    vec3 color = ambient * 2 + accummulatedRadiance;

    vec3 scolor = color / (color + vec3(1.0));
    scolor = pow(scolor, vec3(1.0/2.2));

    color = mix(color, scolor, 0.3);

    outColor = vec4(color, 1.0);
}
