
const float PI = 3.14159265359;
const float DEFAULT_F0 = 0.04;

// Calcula el ratio entre la cantidad entre diffuse y specular.
//  F0: reflejo al mirar directamente a la superficie.
vec3 FreshnelShlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 0.5);
}

//
float DistributionGGX(vec3 normal, vec3 h, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;

    float NdotH = max(dot(normal, h), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = NdotH2 * (a2 - 1.0) + 1.0;
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 normal, vec3 view, vec3 L, float roughness) {
    float NdotV = max(dot(normal, view), 0.0);
    float NdotL = max(dot(normal, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 GetRadiance(vec3 F0, vec3 direction, vec3 view, vec3 normal, vec3 lightColor, vec3 albedoColor, float roughnessFactor, float metallicFactor) {
    vec3 L = -direction;
    vec3 H = normalize(view + L);
    vec3 radiance = lightColor;

    // Cook-Torrance
    float NDF = DistributionGGX(normal, H, roughnessFactor);
    float G = GeometrySmith(normal, view, L, roughnessFactor);
    vec3 F = FreshnelShlick(max(dot(H, view), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallicFactor;

    vec3 num = NDF * G * F;
    float denom = 4.0 * max(dot(normal, view), 0.0) * max(dot(normal, L), 0.0) + 0.0001;
    vec3 specular = num / denom;

    return (kD * albedoColor / PI + specular) * radiance * max(dot(normal, L), 0.0);
}

float CalculateShadowStrength(vec3 cameraSpacePosition, mat4[4] shadowMatrices, vec4 shadowSplits, sampler2DArray dirLightShadowMap) {
    // Cascaded Shadow Map
    const float viewDepth = cameraSpacePosition.z;
    int shadowMapIndex = 0;
    for(int i = 0; i < 4 - 1; i++) {
		if(viewDepth < shadowSplits[i]) {	
			shadowMapIndex = i + 1;
		}
	}

    vec4 fragPosInLightSpace = shadowMatrices[shadowMapIndex] * vec4(inPosition, 1.0);
   
    vec4 projCoords = (fragPosInLightSpace / fragPosInLightSpace.w);
    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    projCoords.y = -projCoords.y;

    const float currentDepth = projCoords.z - 0.01;

    float accumulatedShadow = 0.0;
    const vec2 texelSize = 1.0 / textureSize(dirLightShadowMap, 0).xy;

    for (int x = -1; x < 1; x++) {
        for (int y = -1; y < 1; y++) {
            const vec2 finalCoords = projCoords.xy + vec2(x, y) * texelSize;
            if (finalCoords.x > 1.0 || finalCoords.x < -1.0 || finalCoords.y > 1.0 || finalCoords.y < -1.0) {
                continue;
            }

            float pointDepth = texture(dirLightShadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, shadowMapIndex)).r;
            accumulatedShadow += (currentDepth < pointDepth) ? 1.0 : 0.0;
        }
    }

    return accumulatedShadow / 9.0;
}

vec3 GetShadowmapCascade(vec3 cameraSpacePosition) {
    const float viewDepth = cameraSpacePosition.z;
    int shadowMapIndex = 0;
    for(int i = 0; i < 4 - 1; i++) {
		if(viewDepth < dirLightShadowMat.splits[i]) {	
			shadowMapIndex = i + 1;
		}
	}

    switch (shadowMapIndex) {
        case 0: return vec3(0.0, 0.0, 0.0);
        case 1: return vec3(1.0, 0.0, 0.0);
        case 2: return vec3(0.0, 1.0, 0.0);
        case 3: return vec3(0.0, 0.0, 1.0);

        default: return vec3(1.0, 1.0, 1.0);
    }
    
}

vec3 GetShadowCoordinates() {
      // Cascaded Shadow Map
    const float viewDepth = inFragPosInCameraViewSpace.z;
    int shadowMapIndex = 0;
    for(int i = 0; i < 4 - 1; i++) {
		if(viewDepth < dirLightShadowMat.splits[i]) {	
			shadowMapIndex = i + 1;
		}
	}

    const vec4 fragPosInLightSpace = dirLightShadowMat.matrix[shadowMapIndex] * vec4(inPosition, 1.0);
    vec4 projCoords = (fragPosInLightSpace / fragPosInLightSpace.w);
    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    projCoords.y = -projCoords.y;

    return vec3(projCoords.xy, 1.0);
}
