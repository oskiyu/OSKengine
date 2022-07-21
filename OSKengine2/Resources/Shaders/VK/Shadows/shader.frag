#version 460
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec4 outColor;

void main() {
    outColor = vec4(vec3(gl_FragCoord.z), 1.0);
}
