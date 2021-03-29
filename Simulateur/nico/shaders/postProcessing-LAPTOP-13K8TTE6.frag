#version 330 core

out vec4 FragColor;
in vec2 uv;
uniform sampler2D rendering;
uniform sampler2D depthMap;

void main()
{
    float depth = texture(depthMap, uv).r;
    
    if (depth < 0.99 ){
        FragColor = texture(rendering, uv); 
    }
    else{
        FragColor.rgb = vec3(texture(rendering, uv).r);
        
    }
    FragColor.a = 1;
}