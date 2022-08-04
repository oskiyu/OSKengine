#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPos;

layout(location = 0) out vec2 outTexCoords;

layout (push_constant) uniform Model {
    mat4 cameraMatrix;
    mat4 spriteMatrix;
} camera;

void main() {
    outTexCoords = inPos;

    gl_Position = camera.cameraMatrix * camera.spriteMatrix * vec4(inPos, 0.0, 1.0);
}
