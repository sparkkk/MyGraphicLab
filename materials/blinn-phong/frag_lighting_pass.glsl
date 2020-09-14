#version 330 core
#extension GL_ARB_shading_language_include : require

#include "phong-light-common.glsl"

in vec2 vCoord;

uniform vec3 ViewerPosition;

struct GBufferUniform
{
    sampler2D TextureWorldPosition;
    sampler2D TextureNormal;
    sampler2D TextureTangent;
    sampler2D TextureNormalMap;
    sampler2D TextureDiffuseMap;
    sampler2D TextureSpecularMap;
};

uniform GBufferUniform GBuffer;

uniform LightUniform Light;

layout (location = 0) out vec4 gColor;

void main()
{
    vec4 specularAndShiness = texture(GBuffer.TextureSpecularMap, vCoord);

    vec3 T = normalize(texture(GBuffer.TextureTangent, vCoord).xyz);
    vec3 N = normalize(texture(GBuffer.TextureNormal, vCoord).xyz);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = transpose(mat3(T, B, N));

    GeomParam geomParam;
    geomParam.position = TBN * texture(GBuffer.TextureWorldPosition, vCoord).xyz;
    geomParam.normal = normalize(texture(GBuffer.TextureNormalMap, vCoord).xyz * 2.0 - 1.0);
    geomParam.viewer = normalize(TBN * ViewerPosition - geomParam.position);

    MaterialParam matParam;
    matParam.diffuse = texture(GBuffer.TextureDiffuseMap, vCoord).rgb;
    matParam.specular = specularAndShiness.rgb;
    matParam.shiness = specularAndShiness.a;

    vec3 color = vec3(0.0);
    for (int i = 0; i < Light.LightCount; ++i)
    {
        LightParam lightParam;
        lightParam.position = TBN * Light.Lights[i].Position;
        lightParam.direction = TBN * Light.Lights[i].Direction;
        lightParam.diffuse = Light.Lights[i].DiffuseColor;
        lightParam.specular = Light.Lights[i].SpecularColor;
        lightParam.attenue = Light.Attenue;
        switch (Light.Lights[i].Type)
        {
        case LIGHT_TYPE_D:
            color += calcBlinnPhongLightD(lightParam, geomParam, matParam);
            break;
        case LIGHT_TYPE_P:
            color += calcBlinnPhongLightP(lightParam, geomParam, matParam);
            break;
        }
    }
    vec3 ambient = Light.AmbientColor * matParam.diffuse;
    color += ambient;
    color = clamp(color, 0.0, 1.0);
    gColor = vec4(clamp(color, 0.0, 1.0), 1.0);
}