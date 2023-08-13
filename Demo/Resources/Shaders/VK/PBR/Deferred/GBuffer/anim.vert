#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;
layout(location = 4) in vec3 inTangent;

layout(location = 5) in vec4 inBoneIndices;
layout(location = 6) in vec4 inBoneWeights;

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
    vec4 infos;
} model;

layout (set = 2, binding = 0) buffer Animation {
    mat4 boneMatrices[];
} animation;

void main() {
    outColor = inColor;
    outNormal = normalize(mat3(model.transposedInversedMatrix) * inNormal);
    outTexCoords = inTexCoords;

    const mat4 animationMatrix = animation.boneMatrices[int(inBoneIndices[0])] * inBoneWeights[0]
                                + animation.boneMatrices[int(inBoneIndices[1])] * inBoneWeights[1]
                                + animation.boneMatrices[int(inBoneIndices[2])] * inBoneWeights[2]
                                + animation.boneMatrices[int(inBoneIndices[3])] * inBoneWeights[3];

    outWorldPosition = (model.modelMatrix * animationMatrix * vec4(inPosition, 1.0)).xyz;

    gl_Position = camera.projection * camera.view * vec4(outWorldPosition, 1.0);
}
