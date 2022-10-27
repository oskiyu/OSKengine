#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;

layout(location = 0) out vec4 outWorldPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;
layout(location = 3) out vec2 outTexCoords;

layout(location = 4) out vec4 outPreviousCameraPosition;
layout(location = 5) out vec4 outCurrentCameraPosition;

layout (set = 0, binding = 0) uniform Camera {
    mat4 projection;
    mat4 view;

    vec3 cameraPos;
} camera;

layout (set = 0, binding = 1) uniform PreviousCamera {
    mat4 projection;
    mat4 view;
} previousCamera;

layout (push_constant) uniform Model {
    mat4 modelMatrix;
    mat4 previousModelMatrix;
    vec4 infos;
} model;

void main() {
    outColor = inColor;
    outNormal = normalize(mat3(transpose(inverse(model.modelMatrix))) * inNormal);
    outTexCoords = inTexCoords;

    outWorldPosition = model.modelMatrix * vec4(inPosition, 1.0);

    outPreviousCameraPosition = previousCamera.projection * previousCamera.view * model.previousModelMatrix * vec4(inPosition, 1.0);
    outCurrentCameraPosition = camera.projection * camera.view * model.modelMatrix * vec4(inPosition, 1.0);

    gl_Position = outCurrentCameraPosition;
}
