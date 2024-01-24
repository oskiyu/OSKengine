#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

// Global scene

layout (set = 0, binding = 0) uniform Camera {
    mat4 projection;
    mat4 view;
    mat4 projectionView;

    vec3 cameraPos;
} camera;


// Tables

layout (set = 1, binding = 0) uniform MeshInfo {
    uint positionOffset;
    uint attributesOffset;
    uint animationAttributesOffset;

    uint materialOffset;

    mat4 matrix;
    mat4 previousMatrix;
} meshInfos[];

layout (set = 1, binding = 1) uniform MaterialInfo {
    vec2 metallicRoughness;
    vec4 emissiveColor;

    uint albedoOffset;
    uint normalOffset;
} materialInfos[];


// Images

layout (set = 2, binding = 0) uniform sampler2D images[];


// Inputs

layout(location = 0) flat in uint gdrIndex;

layout(location = 1) in vec4 inWorldPosition;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inColor;
layout(location = 4) in vec2 inTexCoords;

layout(location = 5) in vec4 inPreviousCameraPosition;
layout(location = 6) in vec4 inCurrentCameraPosition;
layout(location = 7) in vec4 inUnjitteredCurrentCameraPosition;

layout(location = 8) in mat3 inTangentMatrix;


// Outputs

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec2 outMetallicRoughness;
layout (location = 3) out vec2 outVelocity;
layout (location = 4) out vec4 outEmissive;


void main() {
    outColor = inColor * texture(images[materialInfos[meshInfos[gdrIndex].materialOffset].albedoOffset], inTexCoords).rgba;
    
    if (outColor.a < 0.75)
        discard;

    const vec2 cameraSpacePreviousPosition = (inPreviousCameraPosition.xy / inPreviousCameraPosition.w) * 0.5 + 0.5;
    const vec2 cameraSpaceCurrentPosition = (inUnjitteredCurrentCameraPosition.xy / inUnjitteredCurrentCameraPosition.w) * 0.5 + 0.5;

    vec2 diff = cameraSpacePreviousPosition - cameraSpaceCurrentPosition;
    diff.y *= -1.0;

    outVelocity = diff;

    vec3 normal = texture(images[materialInfos[meshInfos[gdrIndex].materialOffset].normalOffset], inTexCoords).xyz;
    normal = normal * 2.0 - 1.0;

    normal = normalize(inTangentMatrix * normal);

    // Info packaging: (x.xxx) . (y.yyy)
    outNormal = vec4(normal * 0.5 + 0.5, 1.0);

    outMetallicRoughness = vec2(
        materialInfos[meshInfos[gdrIndex].materialOffset].metallicRoughness.y, 
        materialInfos[meshInfos[gdrIndex].materialOffset].metallicRoughness.x);
}
