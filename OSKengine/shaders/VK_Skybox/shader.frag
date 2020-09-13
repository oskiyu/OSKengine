#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_POINT_LIGHTS 16

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec3 TexCoords;
layout(location = 2) in vec3 fragPos;
layout(location = 3) in vec3 cameraPos;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform samplerCube skybox;

void main() {
    outColor = texture(skybox, TexCoords);
}