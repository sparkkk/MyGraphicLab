#version 330
#extension GL_ARB_shading_language_include : require

#include "pbr-common.glsl"

in vec2 vCoord;
in vec3 vWorldPosition;
in vec3 vViewerPosition;
in mat3 vTBN;


uniform MaterialUniform Material;

uniform LightUniform Light;

uniform float Exposure;

layout (location = 0) out vec4 gColor;

void main()
{
    GeomParam geomParam;
    MaterialParam matParam;

    matParam.albedo = pow(texture(Material.TextureAlbedo, vCoord).rgb, vec3(2.2));
    matParam.metallic = texture(Material.TextureMetallic, vCoord).x;
    matParam.roughness = texture(Material.TextureRoughness, vCoord).x;
    matParam.ao = texture(Material.TextureAmbientOcclusion, vCoord).x;
    matParam.f0 = mix(vec3(0.04), matParam.albedo, matParam.metallic);

    geomParam.position = vWorldPosition;
    geomParam.normal = normalize(texture(Material.TextureNormal, vCoord).xyz * 2.0 - 1.0);
    geomParam.viewer = normalize(vViewerPosition - vWorldPosition);

    vec3 Lo = vec3(0.0);

    for (int i = 0; i < Light.LightCount; ++i)
    {
        LightParam lightParam;
        lightParam.position = vTBN * Light.Lights[i].Position;
        lightParam.direction = vTBN * Light.Lights[i].Direction;
        lightParam.color = Light.Lights[i].Color;
        lightParam.attenue = Light.Attenue;

        switch(Light.Lights[i].Type)
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
