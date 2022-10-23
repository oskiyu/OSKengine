#version 460

#extension GL_EXT_ray_tracing : require

struct RayResult {
    bool isInShadows;
    float distance;
};

layout (location = 0) rayPayloadInEXT RayResult rayResult;

void main() {
    rayResult.isInShadows = false;
    rayResult.distance = 0.0;
}
