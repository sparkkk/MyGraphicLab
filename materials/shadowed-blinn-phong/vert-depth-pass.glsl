#version 330
uniform mat4 PositionTransform;
in vec3 Position;
void main()
{
    gl_Position = PositionTransform * vec4(Position, 1.0);
}