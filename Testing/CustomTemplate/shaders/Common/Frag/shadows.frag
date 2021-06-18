float calculate_shadow(vec4 posInLightSpace, vec3 normal, vec3 lightDir, sampler2D shadowTexture) {
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