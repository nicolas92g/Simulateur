#version 330 core

in vec3 fragPos;

uniform vec3 viewPos;

out vec4 fragmentDir;


void main(){
	fragmentDir.rgb = normalize(viewPos - fragPos);
	fragmentDir.a = 1;
}