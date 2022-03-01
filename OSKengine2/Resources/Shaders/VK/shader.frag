#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 texCoords;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 1) uniform sampler2D stexture;

void main() {
    outColor = color * vec4(texture(stexture, texCoords).xyz, 1.0);
}
