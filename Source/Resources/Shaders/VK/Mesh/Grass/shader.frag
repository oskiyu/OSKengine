#version 460
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec2 outMetallicRoughness;
layout (location = 3) out vec2 outVelocity;
layout (location = 4) out vec4 outEmissive;

layout (location = 0) in VertexInput {
    vec4 position;
    vec4 normal;
    vec4 debugColor;
    // x: height
    // y: y
    vec2 infos;
    uint gid;
} vertexInput;

void main() {
    // Color
    const vec3 grassGreen = vec3(0.804, 0.922, 0.259);

    const float bladeHeight = vertexInput.infos.x;
    const float currentHeight = vertexInput.position.y;

    float selfshadow = pow(currentHeight / bladeHeight, 0.65);
    outColor = vec4(pow(grassGreen, vec3(2.2)) * selfshadow, 1.0);
    // outColor = vertexInput.debugColor;

    outNormal = vec4(vertexInput.normal.xyz * 0.5 + 0.5, 1.0);

    outMetallicRoughness = vec2(
        0.0, 
        0.9);
}
