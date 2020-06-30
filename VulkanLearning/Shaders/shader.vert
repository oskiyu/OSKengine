#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout (push_constant) uniform PushConst{
    mat4 model;
} constants;

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec3 inCol;
layout (location = 2) in vec2 inTexCoords;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 texCoords;

void main() {
    gl_Position = ubo.proj * ubo.view * constants.model * vec4(inPos, 0.0, 1.0);
    fragColor = inCol;
    texCoords = inTexCoords;
}