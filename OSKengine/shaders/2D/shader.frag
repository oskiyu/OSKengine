#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec4 fragColor;
layout (location = 1) in vec2 texCoords;

layout (set = 1, binding = 0) uniform sampler2D textureSampler;

layout (location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor * texture(textureSampler, texCoords));
}