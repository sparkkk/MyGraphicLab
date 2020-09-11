#version 330
uniform mat4 PositionTransform;
uniform mat4 WorldTransform;
in vec3 Position;
in vec2 Coord;
in vec3 Normal;
in vec3 Tangent;
out vec2 vCoord;
out vec3 vNormal;
out vec3 vTangent;
out vec3 vWorldPosition;
void main()
{
    gl_Position = PositionTransform * vec4(Position, 1.0);
    vCoord = Coord;
    vNormal = (WorldTransform * vec4(Normal, 0.0)).xyz;
    vTangent = (WorldTransform * vec4(Tangent, 0.0)).xyz;
    vWorldPosition = (WorldTransform * vec4(Position, 1.0)).xyz;
}