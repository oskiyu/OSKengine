#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;

layout(location = 5) in vec4 inBoneIndices;
layout(location = 6) in vec4 inBoneWeights;

layout (set = 0, binding = 0) uniform DirLight {
    mat4[4] matrix;
    vec4 unused;
} dirLight;

layout (push_constant) uniform Model {
    mat4 modelMatrix;
    int cascadeIndex;
} model;

layout (set = 1, binding = 0) buffer Animation {
    mat4 boneMatrices[];
} animation;

void main() {
    const mat4 animationMatrix = animation.boneMatrices[int(inBoneIndices[0])] * inBoneWeights[0]
                                + animation.boneMatrices[int(inBoneIndices[1])] * inBoneWeights[1]
                                + animation.boneMatrices[int(inBoneIndices[2])] * inBoneWeights[2]
                                + animation.boneMatrices[int(inBoneIndices[3])] * inBoneWeights[3];

    gl_Position = dirLight.matrix[model.cascadeIndex] * model.modelMatrix * animationMatrix * vec4(inPosition, 1.0);
}
