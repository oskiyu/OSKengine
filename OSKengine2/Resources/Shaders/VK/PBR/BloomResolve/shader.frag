#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 texCoords;

layout (set = 0, binding = 0) uniform sampler2DArray bloomTexture;
layout (set = 0, binding = 1) uniform sampler2D sceneTexture;

layout (location = 0) out vec4 outColor;

float GetBrightness(vec4 color) {
    return dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
}

void main() {
    const vec4 sceneColor = texture(sceneTexture, texCoords, 0);
    const vec4 bloomColor = texture(bloomTexture, vec3(texCoords, 0), 0);

    const float brightness = GetBrightness(bloomColor);
    float alpha = bloomColor.a;

    if (brightness < 0.6)
        alpha = 0.0;
    else if (brightness < 0.9)
        alpha = brightness - 0.9;

    outColor = vec4((sceneColor + bloomColor).rgb, sceneColor.a);
}
