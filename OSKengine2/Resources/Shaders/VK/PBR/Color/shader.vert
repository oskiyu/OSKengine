#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;
layout(location = 3) out vec2 outTexCoords;

layout(location = 4) out vec3 outCameraPos;

layout (set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 projection;

    vec3 cameraPos;
} camera;

layout (push_constant) uniform Model {
    mat4 modelMatrix;
} model;

void main() {
    outColor = inColor;
    outNormal = normalize(mat3(transpose(inverse(model.modelMatrix))) * inNormal);
    outTexCoords = inTexCoords;
    outCameraPos = camera.cameraPos;

    vec4 oPos = model.modelMatrix * vec4(inPosition, 1.0);
    outPosition = oPos.xyz;
    gl_Position = camera.projection * camera.view * vec4(oPos);
}
