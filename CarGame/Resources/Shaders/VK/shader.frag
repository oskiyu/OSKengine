#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 texCoords;
layout(location = 4) in vec3 inCameraPos;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 1) uniform sampler2D stexture;

layout (set = 0, binding = 2) uniform Lights {
    vec3 direction;
    vec4 color;
    float intensity;
} lights;

void main() {
    vec3 normal = normalize(inNormal);
	vec3 viewDir = normalize(inCameraPos - inPosition);

    vec3 textureColor = texture(stexture, texCoords).xyz;

    // Directional light
    vec3 directionalLightDirection = normalize(-lights.direction);
	
	//  Diffuse
	float diff = max(dot(normal, directionalLightDirection), 0.0) * lights.intensity;
	vec3 diffuseColor = lights.color.xyz * textureColor * diff;

	//  Specular
	vec3 reflectDir = reflect(-directionalLightDirection, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32/*material.Shininess*/) * lights.intensity;
    vec3 specularColor = lights.color.xyz * spec * textureColor;

    vec3 ambientLight = lights.color.xyz * textureColor * (0.025 + lights.intensity * 0.1);

    outColor = vec4(diffuseColor + specularColor + ambientLight, 1.0); //(color * vec4(texture(stexture, texCoords).xyz, 1.0)) * 0.1 + 
    //outColor = vec4(normal, 1.0);
}
