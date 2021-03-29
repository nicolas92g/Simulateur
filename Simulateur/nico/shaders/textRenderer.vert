#version 330 core
layout(location = 0) in vec4 vertex;

out vec2 TexCoords;

uniform mat4 projection;
uniform bool background;

void main()
{
    if(background){
        gl_Position = projection * vec4(vertex.xy, -0.2, 1.0);
        TexCoords = vertex.zw;
    }
    else{
        gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
        TexCoords = vertex.zw;
    }
}