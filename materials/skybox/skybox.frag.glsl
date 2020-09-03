#version 330

in vec3 vPosition;
layout (location = 0) out vec4 gColor;
uniform samplerCube TextureCube;

void main()
{
    gColor = texture(TextureCube, vPosition);
}