#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "../signed_distance_fields.glsl"

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in flat vec2 inCenterPosition;

layout (set = 0, binding = 0) uniform GlobalInformation {
    vec2 resolution;
} globalInformation;

layout (set = 1, binding = 0) uniform sampler2D stexture;

layout (location = 0) out vec4 outColor;

void main() {
    const vec4 color = inColor * texture(stexture, texCoords);
    const float sdfValue = clamp(-Sdf2D_Box(gl_FragCoord.xy, inCenterPosition, vec2(50)), 0, 1);

    outColor = vec4(color * sdfValue);
}
