#version 330
#extension GL_ARB_shading_language_include : require

#include "phong-light-common.glsl"

in vec2 vCoord;
in vec3 vWorldPosition;
in vec3 vViewerPosition;
in mat3 vTBN;

uniform MaterialUniform Material;

uniform LightUniform Light;

layout (location = 0) out vec4 gColor;

void main()
{
    GeomParam geomParam;
    geomParam.position = vWorldPosition;
    geomParam.normal = normalize(texture(Material.TextureNormal, vCoord).xyz * 2.0 - 1.0);
    geomParam.viewer = normalize(vViewerPosition - vWorldPosition);

    MaterialParam matParam;
    matParam.diffuse = texture(Material.TextureDiffuse, vCoord).rgb;
    matParam.specular = texture(Material.TextureSpecular, vCoord).rgb;
    matParam.shiness = Material.SpecularShiness;

    vec3 color = vec3(0.0);
    
    for (int i = 0; i < Light.LightCount; ++i)
    {
        LightParam lightParam;
        lightParam.position = vTBN * Light.Lights[i].Position;
        lightParam.direction = vTBN * Light.Lights[i].Direction;
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
    gColor = vec4(color, 1.0);
}