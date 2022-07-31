#version 460
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 inPosition;

layout (location = 0) out vec3 outPosition;

layout (push_constant) uniform Info {
    mat4 cameraProj;
    mat4 cameraView;
} info;

void main() {
    outPosition = inPosition;

    mat4 finalView = info.cameraView;
    finalView[3] = vec4(0, 0, 0, 1);

    gl_Position = info.cameraProj * finalView * vec4(inPosition, 1.0);
}
