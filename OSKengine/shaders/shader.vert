#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout (push_constant) uniform PushConst {
    mat4 model;
} constants;

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inCol;
layout (location = 2) in vec2 inTexCoords;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 texCoords;

void main() {
    mat4 matrix = ubo.proj * ubo.view * constants.model;

    gl_Position = matrix * vec4(inPos, 1.0);
    fragColor = inCol;
    texCoords = inTexCoords;
}