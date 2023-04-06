#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;
layout(location = 4) in vec3 inTangent;

layout(location = 0) out vec4 outWorldPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;
layout(location = 3) out vec2 outTexCoords;

layout(location = 4) out vec4 outPreviousCameraPosition;
layout(location = 5) out vec4 outCurrentCameraPosition;
layout(location = 6) out vec4 outUnjitteredCurrentCameraPosition;

layout(location = 7) out mat3 outTangentMatrix;

layout (set = 0, binding = 0) uniform Camera {
    mat4 projection;
    mat4 view;

    vec3 cameraPos;
} camera;

layout (set = 0, binding = 1) uniform PreviousCamera {
    mat4 projection;
    mat4 view;
} previousCamera;

layout (push_constant) uniform Model {
    mat4 modelMatrix;
    mat4 previousModelMatrix;
    vec4 infos;
    vec2 resolution;
} model;

const vec2 jitterValues[] = {
    vec2(0, 0),

    vec2(.9, 1),
    vec2(-.9, -1),
    vec2(-1, .9),
    vec2(1, -.9),
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
    // TAA
    const float jitterFloat = model.infos.z;
    int jitter = 0;
    for (int i = 0; i < 4; i++)
        if (abs(jitterFloat - i) < 0.2)
            jitter = i;

    const vec2 resolution = model.resolution;
	const float deltaWidth = 1 / resolution.x;
	const float deltaHeight = 1 / resolution.y;

    const float jitterScale = 0.55;
    mat4 jitterMatrix = mat4(1.0);
    jitterMatrix[3][0] += (jitterValues[jitter].x) * deltaWidth * jitterScale;
    jitterMatrix[3][1] += (jitterValues[jitter].y) * deltaHeight * jitterScale;

    // Normal
    const mat3 normalMatrix = transpose(inverse(mat3(model.modelMatrix)));

    outColor = inColor;
    outNormal = normalize(normalMatrix * inNormal);
    outTexCoords = inTexCoords;

    outWorldPosition = model.modelMatrix * vec4(inPosition, 1.0);

    // Normal mapping
    const vec3 tangent = normalize(normalMatrix * inTangent);
    const vec3 bitangent = normalize(cross(normalMatrix * inNormal, tangent));
    const mat3 tangentMatrix = mat3(tangent, bitangent, normalMatrix * inNormal);

    outTangentMatrix = tangentMatrix;

    outPreviousCameraPosition = previousCamera.projection * previousCamera.view * model.previousModelMatrix * vec4(inPosition, 1.0);
    outCurrentCameraPosition = jitterMatrix * camera.projection * camera.view * model.modelMatrix * vec4(inPosition, 1.0);
    outUnjitteredCurrentCameraPosition = camera.projection * camera.view * model.modelMatrix * vec4(inPosition, 1.0);

    gl_Position = outCurrentCameraPosition;
}
