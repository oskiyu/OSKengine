#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec4 outColor;
layout(location = 2) out vec2 outTexCoords;

layout (set = 0, binding = 0) uniform Camera {
    mat4 matrix;
} camera;

layout (push_constant) uniform Model {
    mat4 modelMatrix;
} model;

void main() {
    outPosition = inPosition;
    outColor = inColor;
    outTexCoords = inTexCoords;

    gl_Position = model.modelMatrix * vec4(inPosition, 1.0);
}