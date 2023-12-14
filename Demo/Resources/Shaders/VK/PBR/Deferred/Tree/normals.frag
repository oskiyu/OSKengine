#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inNormal;
layout(location = 1) in mat3 inTangentMatrix;

layout (location = 0) out vec4 outNormal;

void main() {
    outNormal = vec4(inNormal * 0.5 + 0.5, 1.0);
}
