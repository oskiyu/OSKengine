#version 460

#extension GL_EXT_ray_tracing : require

struct RayResult {
    bool isInShadows;
    float distance;
};

layout (location = 0) rayPayloadInEXT RayResult rayResult;

hitAttributeEXT vec2 attribs;

void main() {
    rayResult.isInShadows = true;
    rayResult.distance = gl_RayTmaxEXT;
}
