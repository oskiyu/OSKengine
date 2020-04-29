#version 330 core

#define BONES_PER_VERTEX 12
#define MAX_BONES 32

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNor;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in float boneID[BONES_PER_VERTEX];
layout (location = 3) in float boneWeights[BONES_PER_VERTEX];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool Skeleton;
uniform mat4 bones[MAX_BONES];

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main() {
	mat4 bonesTransform = mat4(1.0);
	if (Skeleton){
		bonesTransform = mat4(0.0);
		for (int i = BONES_PER_VERTEX; i > 0; i--){
			bonesTransform += bones[int(boneID[i])] * boneWeights[i];
		}
	}

	gl_Position = projection * view * model * bonesTransform * vec4(aPos, 1.0);
	FragPos = vec3(projection * bonesTransform * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model * bonesTransform))) * aNor;
	TexCoords = aTexCoords;
}