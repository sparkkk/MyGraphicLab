#version 330 core
in vec3 Position;
in vec2 Coord;
uniform mat4 PositionTransform;
out vec2 vCoord;
void main()
{
    gl_Position = PositionTransform * vec4(Position, 1.0);
    vCoord = Coord;
}