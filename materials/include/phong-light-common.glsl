struct MaterialUniform
{
    sampler2D TextureDiffuse;
    sampler2D TextureSpecular;
    sampler2D TextureNormal;
    float SpecularShiness;
};

#define LIGHT_TYPE_D 0
#define LIGHT_TYPE_P 1

struct LightUniformItem
{
    int Type;
    vec3 Position;
    vec3 Direction;
    vec3 DiffuseColor;
    vec3 SpecularColor;
};

struct LightUniform
{
    int LightCount;
    LightUniformItem Lights[4];
    vec3 AmbientColor;
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
    vec3 diffuse;
    vec3 specular;
    float shiness;
};

struct LightParam
{
    vec3 position;
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
    vec3 attenue;
};

vec3 calcPhongLightP(LightParam lightParam, GeomParam geomParam, MaterialParam matParam)
{    
    vec3 lightVec = normalize(lightParam.position - geomParam.position);
    vec3 reflectVec = reflect(-lightVec, geomParam.normal);
    float cosLightNormal = max(dot(lightVec, geomParam.normal), 0.0);
    float cosReflectView = max(dot(reflectVec, geomParam.viewer), 0.0);
    float shinessFactor = pow(cosReflectView, matParam.shiness);
    float dist = length(lightParam.position - geomParam.position);
    vec3 distCo = vec3(1.0, dist, dist * dist);
    float attenuation = dot(lightParam.attenue, distCo);

    vec3 diffuse = cosLightNormal * lightParam.diffuse * matParam.diffuse;
    vec3 specular = shinessFactor * lightParam.specular * matParam.specular;
    return (diffuse + specular) / attenuation;
}

vec3 calcPhongLightD(LightParam lightParam, GeomParam geomParam, MaterialParam matParam)
{    
    vec3 lightVec = normalize(-lightParam.direction);
    vec3 reflectVec = reflect(-lightVec, geomParam.normal);
    float cosLightNormal = max(dot(lightVec, geomParam.normal), 0.0);
    float cosReflectView = max(dot(reflectVec, geomParam.viewer), 0.0);
    float shinessFactor = pow(cosReflectView, matParam.shiness);

    vec3 diffuse = cosLightNormal * lightParam.diffuse * matParam.diffuse;
    vec3 specular = shinessFactor * lightParam.specular * matParam.specular;
    return (diffuse + specular);
}

vec3 calcBlinnPhongLightP(LightParam lightParam, GeomParam geomParam, MaterialParam matParam)
{    
    vec3 lightVec = normalize(lightParam.position - geomParam.position);
    vec3 halfwayVec = normalize(lightVec + geomParam.viewer);
    float cosLightNormal = max(dot(lightVec, geomParam.normal), 0.0);
    float cosReflectView = max(dot(halfwayVec, geomParam.normal), 0.0);
    float shinessFactor = pow(cosReflectView, matParam.shiness);
    float dist = length(lightParam.position - geomParam.position);
    vec3 distCo = vec3(1.0, dist, dist * dist);
    float attenuation = dot(lightParam.attenue, distCo);

    vec3 diffuse = cosLightNormal * lightParam.diffuse * matParam.diffuse;
    vec3 specular = shinessFactor * lightParam.specular * matParam.specular;
    return (diffuse + specular) / attenuation;
}

vec3 calcBlinnPhongLightD(LightParam lightParam, GeomParam geomParam, MaterialParam matParam)
{    
    vec3 lightVec = normalize(-lightParam.direction);
    vec3 halfwayVec = normalize(lightVec + geomParam.viewer);
    float cosLightNormal = max(dot(lightVec, geomParam.normal), 0.0);
    float cosReflectView = max(dot(halfwayVec, geomParam.normal), 0.0);
    float shinessFactor = pow(cosReflectView, matParam.shiness);

    vec3 diffuse = cosLightNormal * lightParam.diffuse * matParam.diffuse;
    vec3 specular = shinessFactor * lightParam.specular * matParam.specular;
    return (diffuse + specular);
}
