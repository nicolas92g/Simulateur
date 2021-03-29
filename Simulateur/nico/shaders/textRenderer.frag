#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec4 textColor = vec4(0,0,0,1);
uniform vec4 backColor = vec4(0,0,0,0.3);

uniform bool background;

void main()
{
    if (background){
        color = backColor;
        return;
    }
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = textColor * sampled;
}