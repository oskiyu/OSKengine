
const float PI = 3.14159265359;
const float DEFAULT_F0 = 0.04;

// Calcula el ratio entre la cantidad entre diffuse y specular.
//  F0: reflejo al mirar directamente a la superficie.
vec3 FreshnelShlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 0.5);
}

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
