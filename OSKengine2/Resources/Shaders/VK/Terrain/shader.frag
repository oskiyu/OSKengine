#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 texCoords;
layout(location = 4) in vec3 inTangent;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 2) uniform sampler2D stexture;

void main() {
    outColor = (color * vec4(texture(stexture, texCoords).xyz, 1.0));// * 0.9 + vec4(normal, 1.0) * 0.1;
}
