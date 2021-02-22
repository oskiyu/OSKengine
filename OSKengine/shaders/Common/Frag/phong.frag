struct DirLight{
	vec3 Direction;
	vec4 Color;
	float Intensity;
};

//Luz puntual.
struct PointLight {
	vec3 Position;
	vec4 Color;

	vec4 infos; //x = intensity, y = radius.
};

vec3 calculate_directional_light(DirLight light, sampler2D diffuseTexture, sampler2D specularTexture, vec2 TexCoords, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(-light.Direction);
	
	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	
	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32/*material.Shininess*/);

	vec3 outputColor = light.Color.xyz * light.Intensity * (vec3(texture(diffuseTexture, TexCoords).xyz) * diff + spec * vec3(texture(specularTexture, TexCoords)));
	
	return outputColor;
}

vec3 calculate_ambient_light(DirLight light, sampler2D diffuseTexture, vec2 TexCoords) {
	return light.Color.xyz * vec3(texture(diffuseTexture, TexCoords)) * (0.025 + light.Intensity * 0.1);
}

vec3 calculate_point_light(PointLight light, sampler2D diffuseTexture, sampler2D specularTexture, vec2 TexCoords, vec3 normal, vec3 fragPos, vec3 viewDir) {
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

vec3 calculate_fog(vec3 cameraPos, vec3 fragPos, vec4 color, vec3 final) {
	float distance_ = abs(length(cameraPos - fragPos));
	float fog = exp(-pow(distance_ * 0.00135, 1.55));
	vec3 _final = mix(color.rgb, final.rgb, fog);

	return _final;
}