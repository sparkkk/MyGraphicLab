#version 330

in vec3 Position;
out vec3 vPosition;
uniform mat4 PositionTransform;
void main()
{
    vPosition = Position;
    gl_Position = PositionTransform * vec4(Position, 1.0);
    //gl_Position = vec4(Position, 1.0);
}