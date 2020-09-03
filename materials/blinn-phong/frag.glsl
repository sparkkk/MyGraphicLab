#version 330

in vec2 vCoord;
in vec3 vWorldPosition;
in vec3 vViewerPosition;
in mat3 vTBN;

struct MaterialParam
{
    sampler2D TextureDiffuse;
    sampler2D TextureSpecular;
    sampler2D TextureNormal;
    float SpecularShiness;
};

struct LightParam
{
    vec3 Position;
    vec3 AmbientColor;
    vec3 DiffuseColor;
    vec3 SpecularColor;
    vec3 Attenue;
};

uniform MaterialParam Material;

uniform int LightCount;
uniform LightParam Lights[4];

layout (location = 0) out vec4 gColor;

void main()
{
    vec3 normalVec = normalize(texture(Material.TextureNormal, vCoord).xyz * 2.0 - 1.0);
    vec3 viewerVec = normalize(vViewerPosition - vWorldPosition);
    vec3 color = vec3(0.0);
    vec3 diffuseMaterialColor = texture(Material.TextureDiffuse, vCoord).rgb;
    vec3 specularMaterialColor = texture(Material.TextureSpecular, vCoord).rgb;
    for (int i = 0; i < LightCount; ++i)
    {
        vec3 LightPosition = vTBN * Lights[i].Position;
        vec3 LightAmbientColor = Lights[i].AmbientColor;
        vec3 LightDiffuseColor = Lights[i].DiffuseColor;
        vec3 LightSpecularColor = Lights[i].SpecularColor;
        vec3 LightAttenue = Lights[i].Attenue;

        vec3 lightVec = normalize(LightPosition - vWorldPosition);
        vec3 halfwayVec = normalize(lightVec + viewerVec);
        float cosLightNormal = max(dot(lightVec, normalVec), 0.0);
        float cosReflectView = max(dot(halfwayVec, normalVec), 0.0);
        float spec = pow(cosReflectView, Material.SpecularShiness);
        float dist = length(LightPosition - vWorldPosition);
        float attenuation = 1.0 / (LightAttenue.x + LightAttenue.y * dist + LightAttenue.z * dist * dist);

        vec3 ambient = LightAmbientColor * diffuseMaterialColor;
        vec3 diffuse = cosLightNormal * LightDiffuseColor * diffuseMaterialColor;
        vec3 specular = spec * LightSpecularColor * specularMaterialColor;

        color += (ambient + diffuse + specular) * attenuation;
    }
    color = clamp(color, 0.0, 1.0);
    gColor = vec4(color, 1.0);
}