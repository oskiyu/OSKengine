
const float PI = 3.14159265359;
const float E = 2.7182818284;
const float DEFAULT_F0 = 0.04;

vec3 GetShadowmapCascade(vec3 cameraSpacePosition, vec4 shadowSplits);
int GetShadowmapIndex(vec3 cameraSpacePosition, vec4 shadowSplits, int numCascades);

// Calcula el ratio entre la cantidad entre diffuse y specular.
//  F0: reflejo al mirar directamente a la superficie.
vec3 FreshnelShlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 0.5);
}

// Distribución de micro-facetas.
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

float GeometryBeckmann(float NdotV, float roughness) {
    const float alpha = roughness * roughness;
    const float a = NdotV / (alpha * sqrt(1 - NdotV * NdotV));

    return (1 - 1.259 * a + 0.396 * a * a)
        / (3.535 * a + 2.181 * a * a);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySchlickGGX_IBL(float NdotV, float roughness) {
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

float GeometrySmith_IBL(vec3 normal, vec3 view, vec3 L, float roughness) {
    float NdotV = max(dot(normal, view), 0.0);
    float NdotL = max(dot(normal, L), 0.0);
    float ggx2  = GeometrySchlickGGX_IBL(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX_IBL(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) 
            * pow(clamp(1.0 - cosTheta, 0.00001, 1.0), 5.0);
}   

vec3 GetRadiance(vec3 F0, vec3 direction, vec3 view, vec3 normal, vec3 lightColor, vec3 albedoColor, float roughnessFactor, float metallicFactor) {
    vec3 L = -direction;
    vec3 H = normalize(view + L);
    vec3 radiance = lightColor;

    // Cook-Torrance
    float D = DistributionGGX(normal, H, max(0.085, roughnessFactor));
    float G = GeometrySmith(normal, view, L, roughnessFactor);
    vec3 F = FreshnelShlick(max(dot(H, view), 0.0), F0);

    vec3 kSpecular = F;
    vec3 kDiffuse = vec3(1.0) - kSpecular;
    kDiffuse *= 1.0 - metallicFactor;
    kSpecular = 1.0 - kDiffuse;

    const vec3 fLambert = albedoColor / PI;
    const vec3 fCookTorrance = (D * F * G) / 4.0 * max(dot(normal, view), 0.0) * max(dot(normal, L), 0.0) + 0.0001;

    return (kDiffuse * fLambert + fCookTorrance) * radiance * max(dot(normal, L), 0.0);
}

vec4 GetShadowCoordinates(mat4[4] shadowMatrices, int cascadeIndex, vec3 fragPosition) {
    const vec4 fragPosInLightSpace = shadowMatrices[cascadeIndex] * vec4(fragPosition, 1.0);
   
    vec4 projCoords = fragPosInLightSpace / fragPosInLightSpace.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;
    projCoords.y = 1.0 + -projCoords.y;

    return projCoords;
}

float CalculateShadowStrength(vec3 cameraSpacePosition, mat4[4] shadowMatrices, vec4 shadowSplits, sampler2DArray dirLightShadowMap, vec3 fragPosition, int numCascades) {
    // Cascaded Shadow Map
    const int shadowMapIndex = GetShadowmapIndex(cameraSpacePosition, shadowSplits, numCascades);

    const vec4 projCoords = GetShadowCoordinates(shadowMatrices, shadowMapIndex, fragPosition);
    const float currentDepth = (projCoords.z) + (0.00025);

    float accumulatedShadow = 0.0;
    const vec2 texelSize = 1.0 / textureSize(dirLightShadowMap, 0).xy;
    
    const float radius = 0.0;
    const int iRadius = int(radius);

    for (int x = -iRadius; x <= iRadius; x++) {
        for (int y = -iRadius; y <= iRadius; y++) {
            const vec2 finalCoords = projCoords.xy + vec2(x, y) * texelSize;
            const float pointDepth = 
                textureLod(dirLightShadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, shadowMapIndex), 0).r;

            accumulatedShadow += float(currentDepth > pointDepth);
        }
    }

    const float diameter = (radius * 2) + 1;
    return accumulatedShadow / (diameter * diameter);
}

int GetShadowmapIndex(vec3 cameraSpacePosition, vec4 shadowSplits, int numCascades) {
    const float viewDepth = -cameraSpacePosition.z;
    
    int index = 0;
    for (int shadowMapIndex = 0; shadowMapIndex <= numCascades; shadowMapIndex++)
        index += int(viewDepth > shadowSplits[shadowMapIndex]);
    
    return index;
}

vec3 GetShadowmapCascade(vec3 cameraSpacePosition, vec4 shadowSplits, mat4[4] shadowMatrices, vec3 fragPosition) {
    const int shadowMapIndex = GetShadowmapIndex(cameraSpacePosition, shadowSplits, 4 - 1);
    const vec4 coords = GetShadowCoordinates(shadowMatrices, shadowMapIndex, fragPosition);

    const float strength = (coords.x > 1 || coords.x < 0 || coords.y > 1 || coords.y < 0)
        ? 0.0
        : 1.0;

    /*return vec3(
        coords.x,
        coords.y,
        shadowMapIndex * 0.25
    );*/

    switch (shadowMapIndex) {
        case 0: return vec3(1.0, 1.0, 1.0) * strength;
        case 1: return vec3(1.0, 0.0, 0.0) * strength;
        case 2: return vec3(0.0, 1.0, 0.0) * strength;
        case 3: return vec3(0.0, 0.0, 1.0) * strength;

        default: return vec3(0.5) * strength;
    }
    
}

/*vec3 GetShadowCoordinates() {
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
*/