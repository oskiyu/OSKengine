#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 color;
layout(location = 1) in vec2 texCoords;

layout (set = 0, binding = 0) uniform sampler2D stexture;

layout (location = 0) out vec4 outColor;

void main() {
    outColor = vec4(color * texture(stexture, texCoords));
}
