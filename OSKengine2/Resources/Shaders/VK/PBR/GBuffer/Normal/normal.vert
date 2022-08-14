#version 460
#extension GL_ARB_separate_shader_objects : enable

// OSK::Vertex3D
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
//layout (location = 2) in vec3 inColor; // unused
//layout (location = 3) in vec3 inTexCoords; // unused

// Output: normal
layout (location = 0) out vec3 outNormal;

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
    outNormal = normalize(mat3(transpose(inverse(model.modelMatrix))) * inNormal);

    gl_Position = camera.projection * camera.view * model.modelMatrix * vec4(inPosition, 1.0);
}
