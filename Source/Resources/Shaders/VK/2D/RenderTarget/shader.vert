#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTexCoords;

layout(location = 0) out vec2 outTexCoords;

void main() {
    gl_Position = vec4(inPos * 2 - 1, 0.0, 1.0);
    
    outTexCoords = inPos;
    outTexCoords.y *= -1.0;
}
