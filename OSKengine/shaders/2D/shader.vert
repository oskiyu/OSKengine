#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (push_constant) uniform Sprite {
    mat4 model;
    vec4 color;
    vec4 texCoords;
} sprite;

layout (set = 0, binding = 0) uniform Camera {
    mat4 cameraMatrix;
} camera;

layout (location = 0) in vec2 inPos;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec2 texCoords;

void main() {
    gl_Position = camera.cameraMatrix * sprite.model * vec4(inPos.xy, 0.0, 1.0);
    fragColor = sprite.color;
    texCoords = sprite.texCoords.xy + inPos.xy * sprite.texCoords.zw;
}
