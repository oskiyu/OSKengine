#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 texCoords;
layout(location = 1) in flat int isHorizontal;

layout (set = 0, binding = 0) uniform sampler2D bloomTexture;

layout (location = 0) out vec4 outColor;

void main() {
    const float weights[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

    const vec2 pixelSize = 1.0 / textureSize(bloomTexture, 0);

    vec4 result = texture(bloomTexture, texCoords, 0) * weights[0];

    if (isHorizontal == 1) {
        for (int i = 0; i < 5; i++) {
            result += texture(bloomTexture, texCoords + vec2(pixelSize.x * i, 0.0), 0) * weights[i];
            result += texture(bloomTexture, texCoords - vec2(pixelSize.x * i, 0.0), 0) * weights[i];
        }
    }
    else {
        for (int i = 0; i < 5; i++) {
            result += texture(bloomTexture, texCoords + vec2(0.0, pixelSize.y * i), 0) * weights[i];
            result += texture(bloomTexture, texCoords - vec2(0.0, pixelSize.y * i), 0) * weights[i];
        }
    }

    outColor = result;
}
