#version 460

#extension GL_EXT_ray_tracing : require

struct RayResult {
    bool isInShadows;
};

layout (location = 0) rayPayloadInEXT RayResult rayResult;

void main() {
    rayResult.isInShadows = false;
}
