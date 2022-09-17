#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform Camera {
    mat4 proj;
    mat4 view;
} camera;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;
layout(location = 3) out vec3 outTexCoords;

void main() {
    outTexCoords = inPosition;
    //outTexCoords.xy *= -1;

    outPosition = inPosition;
    outNormal = inNormal;
    outColor = inColor;

    mat4 finalView = camera.view;
    finalView[3] = vec4(0, 0, 0, 1);

    gl_Position = camera.proj * finalView * vec4(inPosition, 1.0);
}
