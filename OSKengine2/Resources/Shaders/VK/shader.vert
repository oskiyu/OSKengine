#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec4 outColor;

#ifdef OSK_VULKAN
layout (set = 0, binding = 0) uniform Camera {
    mat4 matrix;
} camera;
#else
layout()
#endif

void main() {
    outPosition = inPosition;
    outColor = inColor;

    gl_Position = camera.matrix * vec4(inPosition, 1.0);
}
