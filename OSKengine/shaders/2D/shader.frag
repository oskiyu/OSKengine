#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 texCoords;

layout (binding = 1) uniform sampler2D textureSampler;

layout (location = 0) out vec4 outColor;

void main() {
    outColor = vec4(vec4(fragColor, 1.0) * texture(textureSampler, texCoords));
}