#version 450

// Tipo de primitiva: triángulos, tipo de teselado, counter clockwise.
layout (triangles, fractional_odd_spacing, cw) in;

layout (set = 0, binding = 1) uniform sampler2D heightmap;

layout(location = 1) in vec3 inNormal[];
layout(location = 2) in vec4 inColor[];
layout(location = 3) in vec2 inTexCoords[];

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;
layout(location = 3) out vec2 outTexCoords;

layout (set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 projection;

    vec3 cameraPos;
} camera;

layout (push_constant) uniform Model {
    mat4 modelMatrix;
} model;


void main() {

	const float u = gl_TessCoord.x;
	const float v = gl_TessCoord.y;
	const float w = gl_TessCoord.z;

	outNormal = u * inNormal[0] + v * inNormal[1] + w * inNormal[2];
	outTexCoords = u * inTexCoords[0] + v * inTexCoords[1] + w * inTexCoords[2];
	vec4 pos = u * gl_in[0].gl_Position + v * gl_in[1].gl_Position + w * gl_in[2].gl_Position;

	outColor = inColor[0];

	// Altura del vértice.
	pos.y += texture(heightmap, outTexCoords).x * 0.3;

	// Perspective projection
	gl_Position = camera.projection * camera.view * model.modelMatrix * pos;
}
