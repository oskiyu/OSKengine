#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable


// Camera & global scene

layout (set = 0, binding = 0) uniform Camera {
    mat4 projection;
    mat4 view;
    mat4 projectionView;

    vec3 cameraPos;
} camera;

layout (set = 0, binding = 1) uniform PreviousCamera {
    mat4 projection;
    mat4 view;
    mat4 projectionView;
} previousCamera;


// Tables & attributes

layout (set = 1, binding = 0) uniform MeshInfo {
    uint positionOffset;
    uint attributesOffset;
    uint animationAttributesOffset;

    uint materialOffset;

    mat4 matrix;
    mat4 previousMatrix;
} meshInfos[];

layout (set = 1, binding = 2) uniform Positions {
    vec3 position;
} positions[];

layout (set = 1, binding = 3) uniform Attributes {
    vec3 normal;
    vec4 color;
    vec2 texCoords;
    vec3 tangent;
} attributes[];



layout(location = 0) in uint gdrIndex;

layout(location = 0) out uint outGdrIndex;

layout(location = 1) out vec4 outWorldPosition;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec4 outColor;
layout(location = 4) out vec2 outTexCoords;

layout(location = 5) out vec4 outPreviousCameraPosition;
layout(location = 6) out vec4 outCurrentCameraPosition;
layout(location = 7) out vec4 outUnjitteredCurrentCameraPosition;

layout(location = 8) out mat3 outTangentMatrix;


layout (push_constant) uniform PushConstants {
    vec3 info;
} pushConstants;


const vec2 jitterValues[] = {
    vec2(0, 0),

    vec2(.7, .8),
    vec2(-.7, -.8),
    vec2(-.8, .7),
    vec2(.8, -.7),
};

const vec2 haltonSequence[17] = {
    vec2(.5, .33),
    vec2(.25, .66),
    vec2(.75, .11),
    vec2(.125, .444),
    vec2(0.625000, 0.777778),
    vec2(0.375000, 0.222222),
    vec2(0.875000, 0.555556),
    vec2(0.062500, 0.888889),
    vec2(0.562500, 0.037037),
    vec2(0.312500, 0.370370),
    vec2(0.812500, 0.703704),
    vec2(0.187500, 0.148148),
    vec2(0.687500, 0.481481),
    vec2(0.437500, 0.814815),
    vec2(0.937500, 0.259259),
    vec2(0.031250, 0.592593),

    vec2(0, 0)
};

void main() {
    outGdrIndex = gdrIndex;

    // TAA
    const float jitterFloat = pushConstants.info.z;
    int jitter = 0;
    for (int i = 0; i < 4; i++)
        if (abs(jitterFloat - i) < 0.2)
            jitter = i;

    const vec2 resolution = pushConstants.info.xy;
	const float deltaWidth = 1 / resolution.x;
	const float deltaHeight = 1 / resolution.y;

    const float jitterScale = 1.0;
    mat4 jitterMatrix = mat4(1.0);
    jitterMatrix[3][0] += (jitterValues[jitter].x) * deltaWidth * jitterScale;
    jitterMatrix[3][1] += (jitterValues[jitter].y) * deltaHeight * jitterScale;

    // Normal
    const vec3 inPosition = positions[meshInfos[gdrIndex].positionOffset].position;
    const vec3 inNormal = attributes[meshInfos[gdrIndex].attributesOffset].normal;
    const vec4 inColor = attributes[meshInfos[gdrIndex].attributesOffset].color;
    const vec2 inTexCoords = attributes[meshInfos[gdrIndex].attributesOffset].texCoords;
    const vec3 inTangent = attributes[meshInfos[gdrIndex].attributesOffset].tangent;
    
    const mat4 modelMatrix = meshInfos[gdrIndex].matrix;

    const mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

    outColor = inColor;
    outNormal = normalize(normalMatrix * inNormal);
    outTexCoords = inTexCoords;

    outWorldPosition = modelMatrix * vec4(inPosition, 1.0);

    // Normal mapping
    vec3 normal = normalize(vec3(normalMatrix * inNormal));
    vec3 tangent = normalize(vec3(modelMatrix * vec4(inTangent, 0.0)));
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    const vec3 bitangent = cross(normal, tangent);

    const mat3 tangentMatrix = mat3(tangent, bitangent, normal);

    outTangentMatrix = tangentMatrix;

    outPreviousCameraPosition = previousCamera.projectionView * meshInfos[gdrIndex].previousMatrix * vec4(inPosition, 1.0);
    outCurrentCameraPosition = jitterMatrix * camera.projectionView * meshInfos[gdrIndex].matrix * vec4(inPosition, 1.0);
    outUnjitteredCurrentCameraPosition = camera.projectionView * meshInfos[gdrIndex].matrix * vec4(inPosition, 1.0);

    gl_Position = outCurrentCameraPosition;
}
