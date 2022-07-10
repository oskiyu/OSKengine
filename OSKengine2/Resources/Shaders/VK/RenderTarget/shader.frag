#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 color;
layout(location = 1) in vec2 texCoords;
layout(location = 2) flat in uint imageIndex;

layout (set = 1, binding = 0) uniform sampler2D stexture0;
layout (set = 1, binding = 1) uniform sampler2D stexture1;
layout (set = 1, binding = 2) uniform sampler2D stexture2;

layout (location = 0) out vec4 outColor;

void main() {
    if (imageIndex == 0) {
        outColor = vec4(color * texture(stexture0, texCoords));
    }
    else if (imageIndex == 1) {
        outColor = vec4(color * texture(stexture1, texCoords));
    }
    else {
        outColor = vec4(color * texture(stexture2, texCoords));
    }
}
