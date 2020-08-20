#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_POINT_LIGHTS 16

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 fragPos;
layout(location = 3) in vec3 cameraPos;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D diffuseTexture;
layout(binding = 3) uniform sampler2D specularTexture;

struct PointLight {
	vec3 Position;
	vec4 Color;

	float Intensity;
	float Radius;

	float Constant;
	float Linear;
	float Quadratic;
};
struct DirLight {
	vec3 Direction;

	vec4 Ambient;

	float Intensity;
};

layout(binding = 2) uniform LightsUBO {
    DirLight directional;
    PointLight[MAX_POINT_LIGHTS] points;
} lights;

vec3 calculate_dir_light(DirLight light, vec3 normal, vec3 viewDir);

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
    vec3 norm = normalize(fragNormal);
	vec3 viewDir = normalize(cameraPos - fragPos);

	vec3 final = calculate_dir_light(lights.directional, norm, viewDir);

	for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
		float distance = length(lights.points[i].Position - fragPos);
		//if (lights.points[i].Radius < distance)
			//continue;
		//if (lights.points[i].Intensity == 0.0)
			//break;

		final += calculate_point_light(lights.points[i], norm, fragPos, viewDir);
    }
	
	//outColor = vec4(vec3(dot(norm, viewDir)), 1.0);
	outColor = vec4(final, 1.0);
	//outColor = vec4(vec3(length(fragPos - cameraPos)) / 10, 1.0);
}

vec3 calculate_dir_light(DirLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(-light.Direction);
	
	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	
	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32/*material.Shininess*/) * light.Intensity;

	return light.Ambient.xyz * vec3(texture(diffuseTexture, TexCoords).xyz * 0.025 + vec3(texture(diffuseTexture, TexCoords).xyz) * diff * light.Intensity + spec * light.Intensity * vec3(texture(specularTexture, TexCoords)));
}

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.Position - fragPos);
	
	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	
	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32/*material.Shininess*/);
	
	//attenuation
	float distance = length(light.Position - fragPos) / 5;
	float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * (distance * distance));
	
	if (attenuation > 1)
		attenuation = 1;
	if (attenuation < 0)
		attenuation = 0;

	//combinaci�n
	vec3 diffuse = light.Color.xyz * diff * vec3(texture(diffuseTexture, TexCoords));
	vec3 specular = light.Color.xyz * spec * vec3(texture(specularTexture, TexCoords)) * light.Intensity;
	
	diffuse  *= attenuation;
    specular *= attenuation;

    return (diffuse + specular) * light.Intensity;
}