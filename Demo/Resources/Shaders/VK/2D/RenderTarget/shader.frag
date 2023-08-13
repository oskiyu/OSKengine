#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 texCoords;

layout (set = 0, binding = 0) uniform sampler2D spriteTexture;

layout (location = 0) out vec4 outColor;

void main() {
    outColor = texture(spriteTexture, texCoords);
    gl_FragDepth = gl_FragCoord.z;
}
