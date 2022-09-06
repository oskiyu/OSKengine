#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inWorldPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;

layout (location = 0) out vec4 outWorldPosition;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec4 outColor;

layout (set = 0, binding = 1) uniform DirLightShadowMat {
    mat4[4] matrix;
    vec4 splits;
} dirLightShadowMat;

layout (set = 0, binding = 2) uniform DirLight {
    vec4 directionAndIntensity;
    vec4 color;
} dirLight;

layout (set = 0, binding = 1) uniform sampler2D albedoTexture;

void main() {
    outWorldPosition = vec4(inWorldPosition, 1.0);
    outNormal = vec4(inNormal, 1.0);
    outColor = inColor * texture(albedoTexture, inTexCoords);
}
