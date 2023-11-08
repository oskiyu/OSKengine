#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;

layout (set = 0, binding = 0) uniform DirLight {
    mat4[4] matrix;
    vec4 unused;
    int numCascades;
} dirLight;

layout (push_constant) uniform Model {
    mat4 modelMatrix;
    int cascadeIndex;
} model;

void main() {
    gl_Position = dirLight.matrix[model.cascadeIndex] * model.modelMatrix * vec4(inPosition, 1.0);
}
