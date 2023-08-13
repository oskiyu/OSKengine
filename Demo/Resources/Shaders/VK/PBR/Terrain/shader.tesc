#version 460

#define NUM_VER 3

// Número de vértices por patch.
layout (vertices = NUM_VER) out;

// Inputs de TODOS los vértices procesados.
layout(location = 1) in vec3 inNormal[];
layout(location = 2) in vec4 inColor[];
layout(location = 3) in vec2 inTexCoords[];

// Inputs de los vértices generados.
layout(location = 1) out vec3 outNormal[NUM_VER];
layout(location = 2) out vec4 outColor[NUM_VER];
layout(location = 3) out vec2 outTexCoords[NUM_VER];

layout (set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 projection;

    vec3 cameraPos;
} camera;

layout (push_constant) uniform Model {
    mat4 modelMatrix;
    mat4 transposedInverseModelMatrix;
    vec4 materialInfos;
} pushConstants;

void main() {

    // Se procesa una única vez por patch.
    if (gl_InvocationID == 0) {
        const int minTesselation = 1;
        const int maxTesselation = 6;

        const float minDistance = 2.2;
        const float maxDistance = 8.2;

        // pos.z = distancia desde la cámara.
        vec4 pos0 = camera.view * pushConstants.modelMatrix * gl_in[0].gl_Position; 
        vec4 pos1 = camera.view * pushConstants.modelMatrix * gl_in[1].gl_Position; 
        vec4 pos2 = camera.view * pushConstants.modelMatrix * gl_in[2].gl_Position; 
        
        vec4 middlePos0 = mix(pos2, pos1, 0.5);
        vec4 middlePos1 = mix(pos0, pos2, 0.5);
        vec4 middlePos2 = mix(pos1, pos0, 0.5);

        float clampedDistance0 = clamp((abs(middlePos0.z) - minDistance) / maxDistance, 0.0, 1.0);
        float clampedDistance1 = clamp((abs(middlePos1.z) - minDistance) / maxDistance, 0.0, 1.0);
        float clampedDistance2 = clamp((abs(middlePos2.z) - minDistance) / maxDistance, 0.0, 1.0);

        // Nivel: bordes del triángulo.
        float level0 = mix(maxTesselation, minTesselation, clampedDistance0);
        float level1 = mix(maxTesselation, minTesselation, clampedDistance1);
        float level2 = mix(maxTesselation, minTesselation, clampedDistance2);

        vec4 triMidPosition = (pos0 + pos1 + pos2) / 3;

        gl_TessLevelOuter[0] = level0; // TopLeft
        gl_TessLevelOuter[1] = level1; // Bottom
        gl_TessLevelOuter[2] = level2; // TopRight

        gl_TessLevelInner[0] = mix(level0, mix(level1, level2, 0.5), 0.5); // Center
    }

    gl_out[gl_InvocationID].gl_Position =  gl_in[gl_InvocationID].gl_Position;
	outNormal[gl_InvocationID] = inNormal[gl_InvocationID];
	outTexCoords[gl_InvocationID] = inTexCoords[gl_InvocationID];
    outColor[gl_InvocationID] = inColor[gl_InvocationID];
}
