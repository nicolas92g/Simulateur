#version 330

uniform sampler2D image;

in vec2 TexCoord;
out vec4 FragColor;

void main()
{
    FragColor = texture2D(image, TexCoord);
}