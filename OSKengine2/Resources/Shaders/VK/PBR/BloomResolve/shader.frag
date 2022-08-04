#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 texCoords;

layout (set = 0, binding = 0) uniform sampler2D bloomTexture;
layout (set = 0, binding = 1) uniform sampler2D sceneTexture;

layout (location = 0) out vec4 outColor;

void main() {
    const vec3 sceneColor = texture(sceneTexture, texCoords, 0).rgb;
    const vec3 bloomColor = texture(bloomTexture, texCoords, 0).rgb;

    const float brightness = dot(bloomColor, vec3(0.2126, 0.7152, 0.0722));
    const float alpha = brightness > 0.1 ? 1.0 : 0.0;

    outColor = vec4(sceneColor + bloomColor, alpha);
}
