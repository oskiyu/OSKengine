#version 460

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require

// Vertex Info

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;
layout(location = 4) in vec3 inTangent;


// Output

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;
layout(location = 3) out vec2 outTexCoords;

layout(location = 4) out vec3 outCameraPos;
layout(location = 5) out vec3 fragPosInCameraViewSpace;

layout(location = 6) out mat3 outTangentMatrix;

// Motion Vectors
layout(location = 9) out vec4 outCurrentMotionPosition;
layout(location = 10) out vec4 outPreviousMotionPosition;


// Uniforms
// Ocupados en el fragment shader:
// 0 - 3: matrices de sombras.
// 0 - 4: luz direccional.
// 0 - 5: mapa irradiance.
// 0 - 6: mapas de sombras.
// 0 - 7: mapa specular.
// 0 - 8: lut specular.
// 
// 1 - 0: textura.
// 1 - 1: textura de normales.

// Información de la cámara en el instante actual.
layout (set = 0, binding = 0) 
uniform Camera {
    mat4 projection;
    mat4 view;
    vec3 cameraPos;
} camera;

// Información de la cámara en el frame anterior.
layout (set = 0, binding = 1) 
uniform PreviousCamera {
    mat4 projection;
    mat4 view;
} previousCamera;

// Resolución del frame.
layout (set = 0, binding = 2) 
uniform Res {
    vec2 resolution;
} res;


// Push constant
layout (push_constant) uniform Model {
    mat4 modelMatrix;
    mat4 previousModelMatrix;
    vec4 materialInfosAndJitter;
} pushConstants;


// Constants
const vec2 jitterValues[] = {
    vec2(0, 0),

    vec2(.9, 1),
    vec2(-.9, -1),
    vec2(-1, .9),
    vec2(1, -.9),
};



void main() {
    const mat3 normalMatrix = transpose(inverse(mat3(pushConstants.modelMatrix)));

    outColor = inColor;
    outNormal = normalize(normalMatrix * inNormal);
    outTexCoords = inTexCoords;
    outCameraPos = camera.cameraPos;

    // Normal mapping
    const vec3 tangent = normalize(normalMatrix * inTangent);
    const vec3 bitangent = normalize(cross(normalMatrix * inNormal, tangent));
    const mat3 tangentMatrix = mat3(tangent, bitangent, normalMatrix * inNormal);

    outTangentMatrix = tangentMatrix;

    // Jitter
    const vec2 resolution = res.resolution;
	const float deltaWidth = 1 / resolution.x;
	const float deltaHeight = 1 / resolution.y;

    const float jitterFloat = pushConstants.materialInfosAndJitter.z;
    int jitter = 0;
    for (int i = 0; i < 4; i++)
        if (abs(jitterFloat - i) < 0.2)
            jitter = i;

    const float jitterScale = 0.55;
    mat4 jitterMatrix = mat4(1.0);
    jitterMatrix[3][0] += (jitterValues[jitter].x) * deltaWidth * jitterScale;
    jitterMatrix[3][1] += (jitterValues[jitter].y) * deltaHeight * jitterScale;

    // Motion Vectors
    const vec4 currentUnjitteredCameraSpacePosition = camera.projection * camera.view * pushConstants.modelMatrix * vec4(inPosition, 1.0);
    const vec4 previousCameraSpacePosition = previousCamera.projection * previousCamera.view * pushConstants.previousModelMatrix * vec4(inPosition, 1.0);

    outCurrentMotionPosition = currentUnjitteredCameraSpacePosition;
    outPreviousMotionPosition = previousCameraSpacePosition;

    // Posición en el mundo.
    outPosition = (pushConstants.modelMatrix * vec4(inPosition, 1.0)).xyz;

    // Posición en espacio de view (sin jitter).
    const vec4 temp = camera.view * vec4(outPosition, 1.0);
    fragPosInCameraViewSpace = (temp / temp.w).xyz;

    // Posición respecto a la cámara (con jitter).
    gl_Position = jitterMatrix * camera.projection * vec4(fragPosInCameraViewSpace, 1.0);
}
