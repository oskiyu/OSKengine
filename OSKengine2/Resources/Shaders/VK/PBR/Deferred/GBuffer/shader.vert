#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;

layout(location = 0) out vec3 outWorldPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;
layout(location = 3) out vec2 outTexCoords;

layout (set = 0, binding = 0) uniform Camera {
    mat4 projection;
    mat4 view;

    vec3 cameraPos;
} camera;

layout (push_constant) uniform Model {
    mat4 modelMatrix;
    mat4 transposedInversedMatrix;
} model;

void main() {
    outColor = inColor;
    outNormal = normalize(mat3(model.transposedInversedMatrix) * inNormal);
    outTexCoords = inTexCoords;

    outWorldPosition = (model.modelMatrix * vec4(inPosition, 1.0)).xyz;

    gl_Position = camera.projection * camera.view * vec4(outWorldPosition, 1.0);
}
