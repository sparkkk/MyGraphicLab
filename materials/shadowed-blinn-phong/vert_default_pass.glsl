#version 330
uniform mat4 PositionTransform;
uniform mat4 WorldTransform;
uniform vec3 ViewerPosition;
uniform mat4 LightVP;
in vec3 Position;
in vec2 Coord;
in vec3 Normal;
in vec3 Tangent;
out vec2 vCoord;
out vec3 vWorldPosition;
out vec3 vViewerPosition;
out mat3 vTBN;
out vec4 vLightSpacePosition;
void main()
{
    gl_Position = PositionTransform * vec4(Position, 1.0);
    vec3 T = normalize((WorldTransform * vec4(Tangent, 0.0)).xyz);
    vec3 N = normalize((WorldTransform * vec4(Normal, 0.0)).xyz);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = transpose(mat3(T, B, N));
    vCoord = Coord;
    vWorldPosition = (WorldTransform * vec4(Position, 1.0)).xyz;
    vWorldPosition = TBN * vWorldPosition;
    vViewerPosition = TBN * ViewerPosition;
    vTBN = TBN;
    vLightSpacePosition = LightVP * WorldTransform * vec4(Position, 1.0);
}