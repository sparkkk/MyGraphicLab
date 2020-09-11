#version 330 core
in vec2 vCoord;

uniform vec3 ViewerPosition;

struct GBufferParam
{
    sampler2D TextureWorldPosition;
    sampler2D TextureNormal;
    sampler2D TextureTangent;
    sampler2D TextureNormalMap;
    sampler2D TextureDiffuseMap;
    sampler2D TextureSpecularMap;
};

uniform GBufferParam GBuffer;

struct PointLightParam
{
    vec3 Position;
    vec3 DiffuseColor;
    vec3 SpecularColor;
};

struct LightParam
{
    int LightCount;
    PointLightParam Lights[4];
    vec3 AmbientColor;
    vec3 Attenue;
};

uniform LightParam Light;

layout (location = 0) out vec4 gColor;

void main()
{
    vec3 worldPosition = texture(GBuffer.TextureWorldPosition, vCoord).xyz;
    vec3 normalVec = texture(GBuffer.TextureNormal, vCoord).xyz;
    vec3 tangentVec = texture(GBuffer.TextureTangent, vCoord).xyz;
    vec3 normalVal = normalize(texture(GBuffer.TextureNormalMap, vCoord).xyz * 2.0 - 1.0);
    vec3 diffuse = texture(GBuffer.TextureDiffuseMap, vCoord).rgb;
    vec4 specularAndShiness = texture(GBuffer.TextureSpecularMap, vCoord);
    vec3 specular = specularAndShiness.rgb;
    float shiness = specularAndShiness.a;

    vec3 T = normalize(tangentVec);
    vec3 N = normalize(normalVec);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = transpose(mat3(T, B, N));

    worldPosition = TBN * worldPosition;
    vec3 viewerPosition = TBN * ViewerPosition;

    vec3 viewerVec = normalize(viewerPosition - worldPosition);

    vec3 LightAmbientColor = Light.AmbientColor;
    vec3 LightAttenue = Light.Attenue;
    vec3 color = vec3(0.0);
    for (int i = 0; i < Light.LightCount; ++i)
    {
        vec3 LightPosition = Light.Lights[i].Position;
        vec3 LightDiffuseColor = Light.Lights[i].DiffuseColor;
        vec3 LightSpecularColor = Light.Lights[i].SpecularColor;

        LightPosition = TBN * LightPosition;

        vec3 lightVec = normalize(LightPosition - worldPosition);
        vec3 reflectVec = reflect(-lightVec, normalVal);
        float cosLightNormal = max(dot(lightVec, normalVal), 0.0);
        float cosReflectView = max(dot(reflectVec, viewerVec), 0.0);
        float spec = pow(cosReflectView, shiness);
        float dist = length(LightPosition - worldPosition);
        float attenuation = 1.0 / (LightAttenue.x + LightAttenue.y * dist + LightAttenue.z * dist * dist);

        vec3 diffuse = cosLightNormal * LightDiffuseColor * diffuse;
        vec3 specular = spec * LightSpecularColor * specular;

        color += (diffuse + specular) * attenuation;
    }
    vec3 ambient = LightAmbientColor * diffuse;
    color += ambient;
    color = clamp(color, 0.0, 1.0);
    gColor = vec4(clamp(color, 0.0, 1.0), 1.0);
}