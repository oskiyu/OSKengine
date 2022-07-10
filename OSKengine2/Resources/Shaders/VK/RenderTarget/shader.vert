#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPos;

layout (set = 0, binding = 0) uniform Camera {
    mat4 cameraMatrix;
} camera;

layout (push_constant) uniform Sprite {
    mat4 model;
    vec4 color;
    vec4 texCoords;
    uint imageIndex;
} sprite;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outTexCoords;
layout(location = 2) out uint targetImage;

void main() {
    outColor = sprite.color;
    outTexCoords = sprite.texCoords.xy + inPos.xy * sprite.texCoords.zw;
    targetImage = sprite.imageIndex;

    gl_Position = camera.cameraMatrix * sprite.model * vec4(inPos, 0.0, 1.0);
}
