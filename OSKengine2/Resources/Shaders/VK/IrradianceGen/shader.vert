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
    gl_Position = info.cameraProj * info.cameraView * vec4(inPosition, 1.0);
}
