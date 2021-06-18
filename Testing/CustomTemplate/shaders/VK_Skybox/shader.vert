#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_BONES 64

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;

    vec3 cameraPos;
} camera;

layout (push_constant) uniform PushConst {
    mat4 model;
} model;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inCol;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec3 fragTexCoords;
layout(location = 2) out vec3 fragPos;
layout(location = 3) out vec3 cameraPos;

void main() {
    fragTexCoords = inPosition;
    fragTexCoords.xy *= -1;
    fragPos = inPosition;
    mat4 finalView = camera.view;
    finalView[3] = vec4(0, 0, 0, 1);
    gl_Position = camera.proj * finalView * vec4(inPosition, 1.0);
}