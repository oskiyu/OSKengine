#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec3 inTexCoords;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform samplerCube skybox;

layout (push_constant) uniform Brightness {
    float brightness;
} brightness;

void main() {
    outColor = texture(skybox, inTexCoords) * brightness.brightness;
}
