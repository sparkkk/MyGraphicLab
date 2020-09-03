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

uniform MaterialParam Material;

uniform samplerCube TextureEnv;
uniform mat4 EnvTransform;

layout (location = 0) out vec4 gColor;

void main()
{
    vec3 normalVec = normalize(texture(Material.TextureNormal, vCoord).xyz * 2.0 - 1.0);
    vec3 viewerVec = normalize(vViewerPosition - vWorldPosition);
    mat3 invTBN = transpose(vTBN);
    vec3 viewReflectVec = reflect(-viewerVec, normalVec);
    vec3 envCoord = invTBN * viewReflectVec;
    envCoord = (EnvTransform * vec4(envCoord, 1.0)).xyz;
    vec3 envColor = texture(TextureEnv, envCoord).rgb;
    vec3 color = envColor;
    gColor = vec4(color, 1.0);
}
