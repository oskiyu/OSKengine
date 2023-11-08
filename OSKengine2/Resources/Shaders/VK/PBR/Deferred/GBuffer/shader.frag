#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 inWorldPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;

layout(location = 4) in vec4 inPreviousCameraPosition;
layout(location = 5) in vec4 inCurrentCameraPosition;
layout(location = 6) in vec4 inUnjitteredCurrentCameraPosition;

layout(location = 7) in mat3 inTangentMatrix;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec2 outMetallicRoughness;
layout (location = 3) out vec2 outVelocity;

layout (set = 1, binding = 0) uniform sampler2D albedoTexture;
layout (set = 1, binding = 1) uniform sampler2D normalTexture;

layout (set = 0, binding = 0) uniform Camera {
    mat4 projection;
    mat4 view;
    mat4 projectionView;

    vec3 cameraPos;
} camera;


layout (push_constant) uniform Model {
    mat4 modelMatrix;
    mat4 previousModelMatrix;
    // Metallic Roughness
    vec4 infos;
} model;

// x = r
// y = g

void main() {
    outColor = inColor * texture(albedoTexture, inTexCoords).rgba;
    
    if (outColor.a < 0.75)
        discard;

    const vec2 cameraSpacePreviousPosition = (inPreviousCameraPosition.xy / inPreviousCameraPosition.w) * 0.5 + 0.5;
    const vec2 cameraSpaceCurrentPosition = (inUnjitteredCurrentCameraPosition.xy / inUnjitteredCurrentCameraPosition.w) * 0.5 + 0.5;

    vec2 diff = cameraSpacePreviousPosition - cameraSpaceCurrentPosition;
    diff.y *= -1.0;

    outVelocity = diff;

    vec3 normal = texture(normalTexture, inTexCoords).xyz;
    normal = normal * 2.0 - 1.0;

    normal = normalize(inTangentMatrix * normal);

    // Info packaging: (x.xxx) . (y.yyy)
    outNormal = vec4(normal * 0.5 + 0.5, 1.0);
    outMetallicRoughness = vec2(model.infos.x, model.infos.y);
}
