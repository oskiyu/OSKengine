#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "../Common/Frag/FXAA.frag"

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 texCoords;

layout (set = 0, binding = 0) uniform sampler2D textureSampler;

layout (push_constant) uniform Settings {
    int useFXAA;
    int ScreenSizeX;
    int ScreenSizeY;
    float renderRes;
} settings;

layout (location = 0) out vec4 outColor;

void main() {
    vec3 final = texture(textureSampler, texCoords).xyz;

    if (settings.useFXAA == 1) {
        final = fxaa(textureSampler, gl_FragCoord.st, vec2(settings.ScreenSizeX, settings.ScreenSizeY)).xyz;
    }

    outColor = vec4(final, 1.0);
}
