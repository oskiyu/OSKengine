#version 330 core

struct Material {
	vec3 Ambient;
	sampler2D Diffuse;
	sampler2D Specular;
	float Shininess;
};

uniform Material material;

struct DirLight {
	vec3 Direction;

	vec4 Ambient;

	float Intensity;
};
uniform DirLight dirLight;

uniform vec3 viewPos;

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct PointLight {
	vec3 Position;
	vec4 Color;

	float Intensity;
	float Radius;

	float Constant;
	float Linear;
	float Quadratic;
};

const int MAX_POINT_LIGHTS = 32;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform int NumberOfPointLights;

uniform int Output;

//
vec3 calculate_dir_light(DirLight light, vec3 normal, vec3 viewDir);

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

//
void main() {
	
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 final;
	
	if (Output == 0) { //Renderizado típico.
		final = calculate_dir_light(dirLight, norm, viewDir);

		for (int i = 0; i < NumberOfPointLights; i++) {
			float distance = length(pointLights[i].Position - FragPos);
			if (pointLights[i].Radius < distance)
				break;

			final += calculate_point_light(pointLights[i], norm, FragPos, viewDir);
		}
	}
	else if (Output == 1) { //Vectores normales.
		final = norm;
	}
	else if (Output == 2) {
		final = (FragPos);
	}


    FragColor = vec4(final, 1.0);
}

vec3 calculate_dir_light(DirLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(-light.Direction);
	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);

	return light.Ambient.xyz * (vec3(texture(material.Diffuse, TexCoords).xyz) * dirLight.Intensity * 0.45 + vec3(texture(material.Diffuse, TexCoords).xyz) * diff * dirLight.Intensity + spec * vec3((texture(material.Specular, TexCoords).xyz * dirLight.Ambient.xyz)));
}

vec3 calculate_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.Position - fragPos);
	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Shininess);
	//attenuation
	float distance = length(light.Position - fragPos);
	float attenuation = 1.0 / (light.Constant + light.Linear * distance + light.Quadratic * (distance * distance));
	//combinaci�n
	vec3 diffuse = light.Color.xyz * diff * vec3(texture(material.Diffuse, TexCoords));
	vec3 specular = light.Color.xyz * spec * vec3(texture(material.Specular, TexCoords));
	diffuse  *= attenuation;
    specular *= attenuation;

    return (diffuse + specular) * light.Intensity;
}