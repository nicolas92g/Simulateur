#version 330 core

out vec4 FragColor;
in vec2 uv;

float rand(vec2 co);

void main(){
	FragColor = vec4(rand(uv));
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}