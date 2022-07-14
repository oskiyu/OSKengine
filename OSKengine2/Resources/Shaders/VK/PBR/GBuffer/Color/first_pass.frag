#version 460
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inTexCoords;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inPosition;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec4 outPosition;

layout (set = 0, binding = 1) uniform sampler2D albedoTexture;

void main() {
    outColor = vec4(inColor, 1.0) * texture(albedoTexture, inTexCoords);
    outNormal = vec4(inNormal, 1.0);
}
