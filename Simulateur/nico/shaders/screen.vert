#version 330 core
layout (location = 0) in vec4 vert;

out vec2 uv;

void main()
{
    gl_Position = vec4(vert.x * 2, vert.y * 2, 0.0, 1.0); 
    uv = vec2(vert.z, vert.w);
}  