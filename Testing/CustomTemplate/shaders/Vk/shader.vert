#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_BONES 64
#define MAX_BONES_PER_VERTEX 4

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;

    vec3 cameraPos;
} camera;

layout(binding = 1) uniform BonesUBO {
    mat4 bones[MAX_BONES];
} bones;

layout(binding = 2) uniform DirLightsUBO {
    mat4 lightMat;
} DLight;

layout (push_constant) uniform PushConst {
    mat4 model;
} model;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inCol;
layout(location = 2) in vec2 inTexCoords;
layout(location = 3) in vec3 inNormal;

layout(location = 4) in vec4 inBoneWeights;
layout(location = 5) in ivec4 inBoneIDs;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoords;
layout(location = 2) out vec3 fragPos;
layout(location = 3) out vec3 cameraPos;
layout(location = 4) out vec4 lightSpace;

void main() {
    mat4 bonesMat = bones.bones[inBoneIDs[0]] * inBoneWeights[0];
    bonesMat += bones.bones[inBoneIDs[1]] * inBoneWeights[1];
    bonesMat += bones.bones[inBoneIDs[2]] * inBoneWeights[2];
    bonesMat += bones.bones[inBoneIDs[3]] * inBoneWeights[3];

    gl_Position = camera.proj * camera.view * model.model * bonesMat * vec4(inPosition, 1.0);
    fragPos = (model.model * bonesMat * vec4(inPosition, 1.0)).xyz;
    lightSpace = DLight.lightMat * vec4(fragPos, 1.0);
    fragNormal = mat3(transpose(inverse(model.model * bonesMat))) * inNormal;
    fragTexCoords = inTexCoords;
    cameraPos = camera.cameraPos;
}