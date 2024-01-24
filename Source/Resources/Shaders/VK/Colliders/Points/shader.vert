#version 460

#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) flat out vec4 outColor;

layout (set = 0, binding = 0) uniform Camera {
    mat4 projection;
    mat4 view;

    vec3 cameraPos;
} camera;

layout (push_constant) uniform Model {
    vec4 position;
    vec4 color;
} pushConstants;

void main() {
    outColor = pushConstants.color;

    const vec3 outPosition = pushConstants.position.xyz;
    const vec4 temp = camera.view * vec4(outPosition, 1.0);
    const vec3 fragPosInCameraViewSpace = (temp / temp.w).xyz;

    gl_Position = camera.projection * vec4(fragPosInCameraViewSpace, 1.0);
    gl_PointSize = 10;
}
