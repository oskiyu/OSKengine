#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_POINT_LIGHTS 16

//Frag
layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 fragPos;

//Cámara
layout(location = 3) in vec3 cameraPos;

layout(binding = 1) uniform sampler2D diffuseTexture;
layout(binding = 3) uniform sampler2D specularTexture;


//Output
layout(location = 0) out vec4 outColor;


//Luz puntual.
struct PointLight {
	vec3 Position;
	vec4 Color;

	vec4 infos; //x = intensity, y = radius.
};

//Luz direccional.
struct DirLight {
	vec3 Direction;

	vec4 Ambient;

	float Intensity;
};

//Luces.
layout(binding = 2) uniform LightsUBO {
    DirLight directional;
    PointLight[MAX_POINT_LIGHTS] points;
} lights;

vec3 calculate_dir_light(DirLight light, vec3 normal, vec3 viewDir);

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
    vec3 norm = normalize(fragNormal);
	vec3 viewDir = normalize(cameraPos - fragPos);

	vec3 final = vec3(0.0);
	final = calculate_dir_light(lights.directional, norm, viewDir);

	for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
		float distance = abs(length(lights.points[i].Position - fragPos));
		if (distance > lights.points[i].infos.y)
			continue;

		final += calculate_point_light(lights.points[i], norm, fragPos, viewDir);
    }
	
	//Normal vis.
	//final = vec3(fragNormal / 2 + 0.5);

	outColor = vec4(final, 1.0);
}


/*******************/


vec3 calculate_dir_light(DirLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(-light.Direction);
	
	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	
	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32/*material.Shininess*/);

	//Siempre se ve un poco aunque sea.
	vec3 outputColor = light.Ambient.xyz * vec3(texture(diffuseTexture, TexCoords)) * (0.025 + light.Intensity * 0.1);

	outputColor += light.Ambient.xyz * light.Intensity * (vec3(texture(diffuseTexture, TexCoords).xyz) * diff + spec * vec3(texture(specularTexture, TexCoords)));
	
	return outputColor;
}

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	float distance = length(light.Position - fragPos);
	if (distance > light.infos.y)
		return vec3(0.0);

	vec3 lightDir = normalize(light.Position - fragPos);
	
	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	
	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32/*material.Shininess*/);
	
	//attenuation
	float a  = 0.1;
	float b = 1.0 / (light.infos.y * light.infos.y * 0.001);
	
	float attenuation = 1.0 / (1.0 + a * distance + b * distance * distance);
	
	if (attenuation < 0)
		attenuation = 0;
	if (attenuation > 1)
		attenuation = 1;

	//combinaci�n
	vec3 diffuse = light.Color.xyz * diff * vec3(texture(diffuseTexture, TexCoords));
	vec3 specular = light.Color.xyz * spec * vec3(texture(specularTexture, TexCoords));
	
	diffuse  *= attenuation;
    specular *= attenuation;

    return (diffuse + specular) * light.infos.x;
}