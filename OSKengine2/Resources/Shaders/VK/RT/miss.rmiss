#version 460

#extension GL_EXT_ray_tracing : require

struct RayResult {
    vec4 color;
    float distance;
    vec3 normal;
};

layout (location = 0) rayPayloadInEXT RayResult rayResult;

void main() {
    rayResult.color = vec4(0.5, 0.6, 0.4, 1.0);
    rayResult.distance = -1.0;
    rayResult.normal = vec3(0.0);
}
