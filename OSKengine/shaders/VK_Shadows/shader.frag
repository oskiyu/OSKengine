#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_POINT_LIGHTS 16

//Frag
layout(location = 0) in vec3 fragPos;

//Cámara
layout(location = 1) in vec3 cameraPos;

//Output
layout(location = 0) out vec4 outColor;

void main() {
	float x = length(cameraPos - fragPos);
	outColor = vec4(x, x, x, 1.0);
}