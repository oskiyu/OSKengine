#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;
layout(location = 3) in vec2 inTexCoords;

layout(location = 4) in vec3 inCameraPos;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 1) uniform sampler2D stexture;

layout (set = 0, binding = 2) uniform DirLight {
    vec3 direction;
    vec4 color;
    float intensity;
} dirLight;

void main() {
    vec3 normal = normalize(inNormal);
	vec3 viewDir = normalize(inCameraPos - inPosition);

    vec3 textureColor = texture(stexture, inTexCoords).xyz;

    // Directional light
    vec3 directionalLightDirection = normalize(-dirLight.direction);
	
	//  Diffuse
	float diff = max(dot(normal, directionalLightDirection), 0.0) * dirLight.intensity;
	vec3 diffuseColor = dirLight.color.xyz * textureColor * diff;

	//  Specular
	vec3 reflectDir = reflect(-directionalLightDirection, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32) * dirLight.intensity;
    vec3 specularColor = dirLight.color.xyz * spec;

    vec3 ambientLight = dirLight.color.xyz * textureColor * (0.055 + dirLight.intensity * 0.2);

    outColor = vec4(diffuseColor + specularColor * 1.5 + ambientLight, 1.0);
}
