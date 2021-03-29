#version 330 core

out vec4 FragColor;

in vec2 uv;

uniform sampler2D tex;
uniform vec3 mixedColor;
uniform float transparency;

void main(){
	FragColor = texture(tex, uv);
	FragColor.rgb = FragColor.rgb * mixedColor;
	FragColor.a = transparency * FragColor.a;
}