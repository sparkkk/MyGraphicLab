#version 330
#extension GL_ARB_shading_language_include : require

#include "phong-light-common.glsl"

in vec2 vCoord;
in vec3 vWorldPosition;
in vec3 vViewerPosition;
in mat3 vTBN;
in vec4 vLightSpacePosition;

uniform MaterialUniform Material;

uniform LightUniform Light;

uniform sampler2D ShadowMap;

layout (location = 0) out vec4 gColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    vec2 s = vec2(1.0) / textureSize(ShadowMap, 0);
    float bias = 0.005;
    float d = 0.0;
    d += texture(ShadowMap, projCoords.xy + vec2(-s.x, -s.y)).r < currentDepth - bias ? 1.0 : 0.0;
    d += texture(ShadowMap, projCoords.xy + vec2( 0.0, -s.y)).r < currentDepth - bias ? 1.0 : 0.0;
    d += texture(ShadowMap, projCoords.xy + vec2(+s.x, -s.y)).r < currentDepth - bias ? 1.0 : 0.0;
    d += texture(ShadowMap, projCoords.xy + vec2(-s.x,  0.0)).r < currentDepth - bias ? 1.0 : 0.0;
    d += texture(ShadowMap, projCoords.xy + vec2( 0.0,  0.0)).r < currentDepth - bias ? 1.0 : 0.0;
    d += texture(ShadowMap, projCoords.xy + vec2(+s.x,  0.0)).r < currentDepth - bias ? 1.0 : 0.0;
    d += texture(ShadowMap, projCoords.xy + vec2(-s.x, +s.y)).r < currentDepth - bias ? 1.0 : 0.0;
    d += texture(ShadowMap, projCoords.xy + vec2( 0.0, +s.y)).r < currentDepth - bias ? 1.0 : 0.0;
    d += texture(ShadowMap, projCoords.xy + vec2(-s.x, +s.y)).r < currentDepth - bias ? 1.0 : 0.0;
    // check whether current frag pos is in shadow
    float shadow = d / 9.0;

    return shadow;
}  

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
    float shadow = ShadowCalculation(vLightSpacePosition);
    color *= (1.0 - shadow);
    vec3 ambient = Light.AmbientColor * matParam.diffuse;
    color += ambient;
    color = clamp(color, 0.0, 1.0);
    gColor = vec4(color, 1.0);
}