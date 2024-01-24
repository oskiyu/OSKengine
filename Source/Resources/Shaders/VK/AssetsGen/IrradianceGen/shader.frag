#version 460
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 inPosition;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform sampler2D image;

const vec2 invAtan = vec2(0.1591, 0.3183);

void main() {
    vec3 v = normalize(inPosition);
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;

    outColor = vec4(texture(image, uv).rgb, 1.0);
}
