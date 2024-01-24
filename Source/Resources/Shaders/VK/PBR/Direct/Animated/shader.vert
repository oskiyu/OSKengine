#version 460
#extension GL_ARB_separate_shader_objects : enable

// Vertex Info

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;
layout(location = 4) in vec3 inTangent;

layout(location = 5) in vec4 inBoneIndices;
layout(location = 6) in vec4 inBoneWeights;

// Output

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;
layout(location = 3) out vec2 outTexCoords;

layout(location = 4) out vec3 outCameraPos;
layout(location = 5) out vec3 fragPosInCameraViewSpace;

layout(location = 6) out vec3[3] outTangentMatrix;

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

layout (set = 2, binding = 0) buffer Animation {
    mat4 boneMatrices[];
} animation;

layout (set = 0, binding = 4) uniform PreviousCamera {
    mat4 projection;
    mat4 view;
} previousCamera;

void main() {
    outColor = inColor;

    const mat3 normalMatrix = mat3(pushConstants.transposedInverseModelMatrix);
    
    outNormal = normalize(mat3(pushConstants.transposedInverseModelMatrix) * inNormal);
    outTexCoords = inTexCoords;
    outCameraPos = camera.cameraPos;

    // Normal mapping
    const vec3 tangent = normalize(normalMatrix * inTangent);
    const vec3 bitangent = cross(outNormal, tangent);
    const mat3 tangentMatrix = mat3(tangent, bitangent, outNormal);

    outTangentMatrix[0] = tangentMatrix[0];
    outTangentMatrix[1] = tangentMatrix[2];
    outTangentMatrix[2] = tangentMatrix[2];

    const bool hasAnimation = inBoneWeights[0] + inBoneWeights[1] + inBoneWeights[2] + inBoneWeights[3] > 0;
    const mat4 animationMatrix = hasAnimation
        ? animation.boneMatrices[int(inBoneIndices[0])] * inBoneWeights[0]
                                + animation.boneMatrices[int(inBoneIndices[1])] * inBoneWeights[1]
                                + animation.boneMatrices[int(inBoneIndices[2])] * inBoneWeights[2]
                                + animation.boneMatrices[int(inBoneIndices[3])] * inBoneWeights[3]
                                
        : mat4(1.0);


    const vec3 worldPosition = (pushConstants.modelMatrix * animationMatrix * vec4(inPosition, 1.0)).xyz;
    
    const vec4 temp = camera.view * vec4(worldPosition, 1.0);
    fragPosInCameraViewSpace = (temp / temp.w).xyz;

    gl_Position = camera.projection * vec4(fragPosInCameraViewSpace, 1.0);
    outPosition = worldPosition;
}
