#version 450
#extension GL_ARB_separate_shader_objects : enable

#define MAX_POINT_LIGHTS 16

//Frag
layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 TexCoords;
layout(location = 2) in vec3 fragPos;

//Cámara
layout(location = 3) in vec3 cameraPos;
layout(location = 4) in vec4 lightSpace;

layout(binding = 3) uniform sampler2D diffuseTexture;
layout(binding = 5) uniform sampler2D specularTexture;
layout(binding = 6) uniform sampler2D shadowTexture;


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
layout(binding = 4) uniform LightsUBO {
    DirLight directional;
    PointLight[MAX_POINT_LIGHTS] points;
} lights;

vec3 calculate_dir_light(DirLight light, vec3 normal, vec3 viewDir);

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

float calculate_shadow(vec4 posInLightSpace, vec3 normal, vec3 lightDir);

vec3 calculate_ambient_light(DirLight light);

void main() {
    vec3 norm = normalize(fragNormal);
	vec3 viewDir = normalize(cameraPos - fragPos);

	float shadow = calculate_shadow(lightSpace, norm, normalize(-lights.directional.Direction));

	vec3 final = vec3(0.0);
	final = calculate_ambient_light(lights.directional);
	final += (1 - shadow) * calculate_dir_light(lights.directional, norm, viewDir);

	for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
		float distance = abs(length(lights.points[i].Position - fragPos));
		if (distance > lights.points[i].infos.y)
			continue;

		final += calculate_point_light(lights.points[i], norm, fragPos, viewDir);
    }
	
	//Normal vis.
	//final = vec3(fragNormal / 2 + 0.5);
	//final = vec3(shadow, shadow, shadow);
	
	float distance_ = abs(length(cameraPos - fragPos));
	float fog = exp(-pow(distance_ * 0.00135, 1.55));
	final = mix(lights.directional.Ambient.xyz * 0.5, final, fog);

	outColor = vec4(final, 1.0);
}


/*******************/

vec3 calculate_ambient_light(DirLight light) {
	//Siempre se ve un poco aunque sea.
	return light.Ambient.xyz * vec3(texture(diffuseTexture, TexCoords)) * (0.025 + light.Intensity * 0.1);
}

vec3 calculate_dir_light(DirLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(-light.Direction);
	
	//diffuse
	float diff = max(dot(normal, lightDir), 0.0) * light.Intensity;
	
	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32/*material.Shininess*/) * light.Intensity;

	//Siempre se ve un poco aunque sea.
	vec3 outputColor = light.Ambient.xyz * light.Intensity * (vec3(texture(diffuseTexture, TexCoords).xyz) * diff + spec * vec3(texture(specularTexture, TexCoords)));
	
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

float calculate_shadow(vec4 posInLightSpace, vec3 normal, vec3 lightDir) {
	vec3 coords = posInLightSpace.xyz / posInLightSpace.w;
	coords.xy = coords.xy * 0.5 + 0.5;

	float depth = texture(shadowTexture, coords.xy).r;

	float currentDepth = coords.z - 0.01;
	
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
	float shadow = currentDepth - bias > depth  ? 1.0 : 0.0;  

	
	vec2 texelSize = 1.0 / textureSize(shadowTexture, 0);
	for(int x = -1; x <= 1; ++x) {
		for(int y = -1; y <= 1; ++y) {
			if (coords.x > 1.0 || coords.x < 0.0 || coords.y > 1.0 || coords.y < 0.0)
				continue;

			float pcfDepth = texture(shadowTexture, coords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth >= pcfDepth  ? 1.0 : 0.0;        
		}    
	}

	shadow /= 9.0;

	return shadow;
}
