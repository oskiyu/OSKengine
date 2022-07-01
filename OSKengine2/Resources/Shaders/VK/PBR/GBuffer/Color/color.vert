#version 460
#extension GL_ARB_separate_shader_objects : enable

// OSK::Vertex3D
layout (location = 0) in vec3 inPosition;
//layout (location = 1) in vec3 inNormal; //unused
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec3 inTexCoords;

// Output: vertex color
layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outTexCoords;

// Camera UBO
layout (set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 projection;

    vec3 cameraPos;
} camera;

layout (push_constant) uniform Model {
    mat4 modelMatrix;
} model;

void main() {
    outColor = inColor;

    gl_Position = camera.projection * camera.view * model.modelMatrix * vec4(inPosition, 1.0);
}
