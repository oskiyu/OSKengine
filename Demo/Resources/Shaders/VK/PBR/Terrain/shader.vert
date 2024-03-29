#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;
layout(location = 4) in vec3 inTangent;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;
layout(location = 3) out vec2 outTexCoords;

layout (set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 projection;

    vec3 cameraPos;
} camera;

void main() {
    outPosition = inPosition;
    outColor = inColor;
    outNormal = inNormal;
    outTexCoords = inTexCoords;
    
    gl_Position = vec4(inPosition, 1.0);
}
