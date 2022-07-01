#version 460
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 inNormal;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 1) uniform sampler2D stexture;

void main() {
    outColor = vec4(inNormal, 1.0);
}
