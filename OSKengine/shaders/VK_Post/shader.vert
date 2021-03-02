#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inCol;
layout (location = 2) in vec2 inTexCoords;
layout (location = 3) in vec3 inNor;

layout (location = 0) out vec3 fragColor;
layout (location = 1) out vec2 texCoords;

void main() {
    fragColor = inCol;
    texCoords = inTexCoords;
    gl_Position = vec4(inPos.xy, 0.0, 1.0);
}
