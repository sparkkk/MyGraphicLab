#version 330 core
#extension GL_ARB_shading_language_include : require

#include "pbr-common.glsl"

in vec2 vCoord;

uniform vec3 ViewerPosition;

struct GBufferUniform
{
    sampler2D TextureWorldPosition;
    sampler2D TextureNormal;
    sampler2D TextureTangent;
    sampler2D TextureNormalMap;
    sampler2D TextureAlbedoMap;
    sampler2D TextureMRAMap;
};

uniform GBufferUniform GBuffer;

uniform LightUniform Light;

uniform float Exposure;

layout (location = 0) out vec4 gColor;

void main()
{
    vec3 T = normalize(texture(GBuffer.TextureTangent, vCoord).xyz);
    vec3 N = normalize(texture(GBuffer.TextureNormal, vCoord).xyz);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = transpose(mat3(T, B, N));

    GeomParam geomParam;
    geomParam.position = TBN * texture(GBuffer.TextureWorldPosition, vCoord).xyz;
    geomParam.normal = normalize(texture(GBuffer.TextureNormalMap, vCoord).xyz * 2.0 - 1.0);
    geomParam.viewer = normalize(TBN * ViewerPosition - geomParam.position);

    MaterialParam matParam;
    matParam.albedo = pow(texture(GBuffer.TextureAlbedoMap, vCoord).rgb, vec3(2.2));
    vec3 mra = texture(GBuffer.TextureMRAMap, vCoord).rgb;
    matParam.metallic = mra.r;
    matParam.roughness = mra.g;
    matParam.ao = mra.b;
    matParam.f0 = mix(vec3(0.04), matParam.albedo, matParam.metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < Light.LightCount; ++i)
    {
        LightParam lightParam;
        lightParam.position = TBN * Light.Lights[i].Position;
        lightParam.direction = TBN * Light.Lights[i].Direction;
        lightParam.color = Light.Lights[i].Color;
        lightParam.attenue = Light.Attenue;
        switch (Light.Lights[i].Type)
        {
        case LIGHT_TYPE_D:
            Lo += calcPbrLightD(lightParam, geomParam, matParam);
            break;
        case LIGHT_TYPE_P:
            Lo += calcPbrLightP(lightParam, geomParam, matParam);
            break;
        }
    }
    vec3 ambient = vec3(0.03) * matParam.albedo * matParam.ao;
    vec3 color = ambient + Lo;
	
    color = ToneMapping(color, Exposure);
   
    gColor = vec4(color, 1.0);
}