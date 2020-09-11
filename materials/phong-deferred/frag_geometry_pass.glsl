#version 330

struct MaterialParam
{
    sampler2D TextureDiffuse;
    sampler2D TextureSpecular;
    sampler2D TextureNormal;
    float SpecularShiness;
};

uniform MaterialParam Material;

in vec2 vCoord;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vWorldPosition;

layout (location = 0) out vec4 gWorldPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gTangent;
layout (location = 3) out vec4 gNormalMap;
layout (location = 4) out vec4 gDiffuseMap;
layout (location = 5) out vec4 gSpecularMap;

void main()
{
    gWorldPosition = vec4(vWorldPosition, 1.0);
    gNormal = vec4(normalize(vNormal), 1.0);
    gTangent = vec4(normalize(vTangent), 1.0);
    gNormalMap = texture(Material.TextureNormal, vCoord);
    gDiffuseMap = texture(Material.TextureDiffuse, vCoord);
    gSpecularMap = vec4(texture(Material.TextureSpecular, vCoord).rgb, Material.SpecularShiness);
}