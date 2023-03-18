#version 460

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;
layout(location = 4) in vec3 inTangent;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;
layout(location = 3) out vec2 outTexCoords;

layout(location = 4) out vec3 outCameraPos;
layout(location = 5) out vec3 fragPosInCameraViewSpace;

layout (set = 0, binding = 0) uniform Camera {
    mat4 projection;
    mat4 view;

    vec3 cameraPos;
} camera;

layout (push_constant) uniform Model {
    mat4 modelMatrix;
    mat4 transposedInverseModelMatrix;
    vec4 materialInfos;
} pushConstants;

void main() {
    outColor = inColor;
    outNormal = normalize(mat3(transpose(inverse(pushConstants.modelMatrix))) * inNormal);
    outTexCoords = inTexCoords;
    outCameraPos = camera.cameraPos;

    outPosition = (pushConstants.modelMatrix * vec4(inPosition, 1.0)).xyz;
    const vec4 temp = camera.view * vec4(outPosition, 1.0);
    fragPosInCameraViewSpace = (temp / temp.w).xyz;

    gl_Position = camera.projection * vec4(fragPosInCameraViewSpace, 1.0);
}
