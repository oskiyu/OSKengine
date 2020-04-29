#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 FragPos;

void main() {
	FragPos = vec4(aPos.x, aPos.y, 0.0, 1.0).xyz;
	gl_Position = vec4(FragPos, 1.0);
	TexCoords = aTexCoords;
}
