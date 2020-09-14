struct MaterialUniform
{
    sampler2D TextureAlbedo;
    sampler2D TextureNormal;
    sampler2D TextureMetallic;
    sampler2D TextureRoughness;
    sampler2D TextureAmbientOcclusion;
};

#define LIGHT_TYPE_D 0
#define LIGHT_TYPE_P 1

struct LightUniformItem
{
    int Type;
    vec3 Position;
    vec3 Direction;
    vec3 Color;
};

struct LightUniform
{
    int LightCount;
    LightUniformItem Lights[4];
    vec3 Attenue;
};

struct GeomParam
{
    vec3 position;
    vec3 normal;
    vec3 viewer;
};

struct MaterialParam
{
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    vec3 f0;
};

struct LightParam
{
    vec3 position;
    vec3 direction;
    vec3 color;
    vec3 attenue;
};

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    const float PI = 3.14159265359;
    float a = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 ToneMapping(vec3 color, float exposure)
{
    color = vec3(1.0) - exp(-color * exposure);
    color = pow(color, vec3(1.0 / 2.2));
    return color;
}

vec3 calcPbrLightD(LightParam lightParam, GeomParam geomParam, MaterialParam matParam)
{
    const float PI = 3.14159265359;
    vec3 N = geomParam.normal;
    vec3 L = normalize(-lightParam.direction);
    vec3 V = geomParam.viewer;
    vec3 H = normalize(V + L);

    float NLdot = max(dot(L, N), 0.0);

    vec3 radiance = lightParam.color;
    
    float NDF = DistributionGGX(N, H, matParam.roughness);
    float G   = GeometrySmith(N, V, L, matParam.roughness);
    vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), matParam.f0);

    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - matParam.metallic;
    vec3 diffuse = kD * matParam.albedo / PI;
    
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular     = numerator / max(denominator, 0.001);

    return (diffuse + specular) * NLdot * radiance;
}

vec3 calcPbrLightP(LightParam lightParam, GeomParam geomParam, MaterialParam matParam)
{
    const float PI = 3.14159265359;
    vec3 N = geomParam.normal;
    vec3 L = normalize(lightParam.position - geomParam.position);
    vec3 V = geomParam.viewer;
    vec3 H = normalize(V + L);

    float NLdot = max(dot(L, N), 0.0);

    float dist = length(lightParam.position - geomParam.position);
    vec3 distCo = vec3(1.0, dist, dist * dist);
    float attenuation = dot(lightParam.attenue, distCo);
    vec3 radiance = lightParam.color / attenuation;
    
    float NDF = DistributionGGX(N, H, matParam.roughness);
    float G   = GeometrySmith(N, V, L, matParam.roughness);
    vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), matParam.f0);

    vec3 kD = vec3(1.0) - F;
    kD *= 1.0 - matParam.metallic;
    vec3 diffuse = kD * matParam.albedo / PI;
    
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular     = numerator / max(denominator, 0.001);

    return (diffuse + specular) * NLdot * radiance;
}