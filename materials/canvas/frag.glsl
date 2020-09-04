#version 330 core
in vec2 vCoord;
uniform sampler2D Texture1;
layout (location = 0) out vec4 color1;
void main()
{
    color1 = texture(Texture1, vCoord);
}