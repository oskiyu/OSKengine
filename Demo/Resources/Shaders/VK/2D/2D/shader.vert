#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPos;

layout (push_constant) uniform Sprite {
    mat4 model;
    vec4 color;
    vec4 texCoords;
} sprite;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outTexCoords;

void main() {
    outColor = sprite.color;
    outTexCoords = sprite.texCoords.xy + inPos.xy * sprite.texCoords.zw;

    gl_Position = sprite.model * vec4(inPos, 0.0, 1.0);
}
