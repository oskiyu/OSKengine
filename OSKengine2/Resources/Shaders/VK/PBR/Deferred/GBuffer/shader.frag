#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 inWorldPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;

layout(location = 4) in vec4 inPreviousCameraPosition;
layout(location = 5) in vec4 inCurrentCameraPosition;

layout (location = 0) out vec4 outWorldPosition;
layout (location = 1) out vec4 outColor;
layout (location = 2) out vec4 outNormal;
layout (location = 3) out vec4 outVelocity;

layout (set = 1, binding = 0) uniform sampler2D albedoTexture;

layout (push_constant) uniform Model {
    mat4 modelMatrix;
    mat4 previousModelMatrix;
    vec4 infos;
} model;

// x = r
// y = g

void main() {
    outWorldPosition = vec4(inWorldPosition.xyz, 1.0);
    outColor = inColor * vec4(texture(albedoTexture, inTexCoords).rgb, 1.0);

    const vec2 cameraSpacePreviousPosition = (inPreviousCameraPosition.xy / inPreviousCameraPosition.w) * 0.5 + 0.5;
    const vec2 cameraSpaceCurrentPosition = (inCurrentCameraPosition.xy / inCurrentCameraPosition.w) * 0.5 + 0.5;

    vec2 diff = cameraSpacePreviousPosition - cameraSpaceCurrentPosition;
    diff.y *= -1.0;

    outVelocity = vec4(diff, 0.0 ,1.0);

    // Info packaging: (x.xxx) . (y.yyy)
    outNormal = vec4(inNormal, (model.infos.x * 1000) + (model.infos.y * 0.1));
}
