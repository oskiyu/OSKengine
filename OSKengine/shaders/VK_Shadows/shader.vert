#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_BONES 64
#define MAX_BONES_PER_VERTEX 4

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 proj2D;

    vec3 cameraPos;
} camera;

layout(binding = 1) uniform Bones {
    mat4 bones[MAX_BONES];
} bones;

layout(binding = 2) uniform Lights {
    mat4 lightMat;
} lights;

layout (push_constant) uniform PushConst {
    mat4 model;
} model;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inCol;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in vec3 inNormal;

layout(location = 4) in vec4 inBoneWeights;
layout(location = 5) in ivec4 inBoneIDs;

layout(location = 0) out vec3 fragPos;
layout(location = 1) out vec3 cameraPos;

void main() {
    mat4 bonesMat = bones.bones[inBoneIDs[0]] * inBoneWeights[0];
    bonesMat += bones.bones[inBoneIDs[1]] * inBoneWeights[1];
    bonesMat += bones.bones[inBoneIDs[2]] * inBoneWeights[2];
    bonesMat += bones.bones[inBoneIDs[3]] * inBoneWeights[3];  

    mat4 mat = lights.lightMat;

    gl_Position = mat * model.model * bonesMat * vec4(inPosition, 1.0);
    fragPos = (mat * model.model * bonesMat * vec4(inPosition, 1.0)).xyz;
    cameraPos = camera.cameraPos;
}