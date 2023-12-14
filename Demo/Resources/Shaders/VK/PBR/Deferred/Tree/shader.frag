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
layout (location = 4) out vec4 outEmissive;

layout(origin_upper_left) in vec4 gl_FragCoord;

layout (set = 1, binding = 0) uniform sampler2D albedoTexture;
layout (set = 1, binding = 1) uniform sampler2D normalTexture;

layout (set = 2, binding = 0) uniform sampler2D preCalculatedNormalTexture;

layout (set = 0, binding = 0) uniform Camera {
    mat4 projection;
    mat4 view;
    mat4 projectionView;

    vec3 cameraPos;
} camera;

layout (set = 1, binding = 2) uniform MaterialInfo{
    vec4 emissiveColor;
    vec2 roughnessMetallic;
} materialInfo;


layout (push_constant) uniform Model {
    mat4 modelMatrix;
    mat4 previousModelMatrix;
    vec2 resolution;
    float jitterIndex;
} model;

// x = r
// y = g

void main() {
    outColor = inColor * texture(albedoTexture, inTexCoords).rgba;
    
    const vec3 view = normalize(inWorldPosition.xyz - camera.cameraPos);

    const float distance = length(inWorldPosition.xyz - camera.cameraPos);
    
    const float nDot = abs(dot(view, inNormal));

    float calculatedAngleThreshold = nDot > 0.1
        ? 0.0
        : mix(0, 0.8, nDot * 10);

    if (outColor.a < 0.75 - distance * 0.0071 - calculatedAngleThreshold)
        discard;

    const vec2 cameraSpacePreviousPosition = (inPreviousCameraPosition.xy / inPreviousCameraPosition.w) * 0.5 + 0.5;
    const vec2 cameraSpaceCurrentPosition = (inUnjitteredCurrentCameraPosition.xy / inUnjitteredCurrentCameraPosition.w) * 0.5 + 0.5;

    vec2 diff = cameraSpacePreviousPosition - cameraSpaceCurrentPosition;
    diff.y *= -1.0;

    outVelocity = diff;

    vec3 geometryNormal = inNormal;
    geometryNormal *= -sign(dot(geometryNormal, view));

    vec3 normal = texture(normalTexture, inTexCoords).xyz;
    normal = normal * 2.0 - 1.0;
    normal = normalize(inTangentMatrix * normal);

    const float _dot = dot(normal, view);
    normal *= -sign(_dot);
    
    normal = mix(normal, geometryNormal, 0.85);

    vec3 preCalculatedNormal = textureLod(preCalculatedNormalTexture, gl_FragCoord.xy / model.resolution.xy, 0).xyz * 2.0 - 1.0;
    preCalculatedNormal *=-sign(dot(preCalculatedNormal, view));

    normal = mix(normal, preCalculatedNormal, 0.5);

    // Info packaging: (x.xxx) . (y.yyy)
    outNormal = vec4(normal * 0.5 + 0.5, 1.0);

    outMetallicRoughness = vec2(
        materialInfo.roughnessMetallic.y, 
        materialInfo.roughnessMetallic.x);

    outEmissive = materialInfo.emissiveColor;
}
