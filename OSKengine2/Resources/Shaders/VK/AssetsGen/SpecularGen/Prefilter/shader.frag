#version 460
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 inPosition;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform samplerCube image;

layout (push_constant) uniform Info {
    mat4 cameraProj;
    mat4 cameraView;
    float roughness;
} info;

const float PI = 3.14159265359;


vec2 Hammersley(uint i, uint N);
vec3 ImportanceSampleGGX(vec2 random, vec3 normal, float roughness) {
    const float roughtness2 = roughness * roughness;
    const float roughtness4 = roughtness2 * roughtness2;

    // Coordenadas esféricas
    const float phi = 2.0 * PI * random.x;
    const float cosTheta = sqrt((1.0 - random.y) / (1.0 + (roughtness4 - 1.0) * random.y));
    const float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    // A coordenadas cartesianas
    const vec3 H = vec3(
        cos(phi) * sinTheta,
        sin(phi) * sinTheta,
        cosTheta
    );

    // A espacio del mundo
    const vec3 up = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    const vec3 tangent = normalize(cross(up, normal));
    const vec3 bitangent = cross(normal, tangent);

    const vec3 sampleCoords = tangent * H.x + bitangent * H.y + normal * H.z;

    return normalize(sampleCoords);
}


void main() {
    const vec3 pos = vec3(inPosition.x, -inPosition.y, inPosition.z); // ? -inPosition.y ?
    const vec3 normal = normalize(pos);
    
    const uint numSamples = 1024;

    vec3 color = vec3(0.0);
    float sumWeight = 0.0;

    const float maxHdrRange = 2.5;

    for (uint i = 0; i < numSamples; i++) {
        const vec2 random = Hammersley(i, numSamples);
        const vec3 H = ImportanceSampleGGX(random, normal, info.roughness);
        const vec3 L = normalize(2.0 * dot(H, normal) * H - normal);

        const float weight = max(dot(normal, L), 0.0);
        if (weight > 0.0) {
            color += texture(image, L).rgb * weight;
            sumWeight += weight;
        }
    }

    const vec3 totalColor = clamp(color / sumWeight, vec3(0), vec3(maxHdrRange));
    outColor = vec4(totalColor, 1.0);
}


// RANDOM

float RadicalInverse_VdC(uint bits)  {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N) {
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}  
