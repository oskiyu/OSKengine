#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_BONES 64
#define MAX_BONES_PER_VERTEX 4

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    mat4 proj2D;
    mat4 bones[MAX_BONES];

    vec3 cameraPos;
} camera;

layout(binding = 1) uniform Lights {
    mat4 lightMats[6];
} lights;

layout (push_constant) uniform PushConst {
    mat4 model;
    uint face;
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
    mat4 bonesMat = camera.bones[inBoneIDs[0]] * inBoneWeights[0];
    bonesMat += camera.bones[inBoneIDs[1]] * inBoneWeights[1];
    bonesMat += camera.bones[inBoneIDs[2]] * inBoneWeights[2];
    bonesMat += camera.bones[inBoneIDs[3]] * inBoneWeights[3];  

    mat4 mat = lights.lightMats[model.face];

    gl_Position = mat * model.model * bonesMat * vec4(inPosition, 1.0);
    fragPos = (mat * model.model * bonesMat * vec4(inPosition, 1.0)).xyz;
    cameraPos = camera.cameraPos;
}