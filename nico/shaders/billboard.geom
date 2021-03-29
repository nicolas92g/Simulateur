#version 330

layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

uniform mat4 VP;

uniform vec3 up;
uniform vec3 viewPos;
uniform bool followUpAxis = true;

out vec2 TexCoord;

void main()
{
    vec3 Pos = gl_in[0].gl_Position.xyz;
    vec3 right = cross(normalize(viewPos - Pos), up);

//    if(followUpAxis){
//        gl_Position = VP * vec4(Pos + right * 0.5 + up * 0.5, 1.0);
//        TexCoord = vec2(1.0, 1.0);
//        EmitVertex();
//
//        gl_Position = VP * vec4(Pos + right * 0.5 + up * -0.5, 1.0);
//        TexCoord = vec2(1.0, 0.0);
//        EmitVertex();
//
//        gl_Position = VP * vec4(Pos + right * -0.5 + up * 0.5, 1.0);
//        TexCoord = vec2(0.0, 1.0);
//        EmitVertex();
//
//        gl_Position = VP * vec4(Pos + right * -0.5 + up * -0.5, 1.0);
//        TexCoord = vec2(0.0, 0.0);
//        EmitVertex();
//
//        EndPrimitive();
//    }
//    else{
//
//        gl_Position = VP * vec4(Pos + right * 0.5, 1.0);
//        TexCoord = vec2(1.0, 1.0);
//        EmitVertex();
//
//        gl_Position = VP * vec4(Pos + right * 0.5, 1.0);
//        TexCoord = vec2(1.0, 0.0);
//        EmitVertex();
//
//        gl_Position = VP * vec4(Pos + right * -0.5, 1.0);
//        TexCoord = vec2(0.0, 1.0);
//        EmitVertex();
//
//        gl_Position = VP * vec4(Pos + right * -0.5, 1.0);
//        TexCoord = vec2(0.0, 0.0);
//        EmitVertex();
//
//        EndPrimitive();
//    }
    gl_Position = VP * vec4(Pos + right * 0.5 + up * 0.5, 1.0);
    TexCoord = vec2(1.0, 1.0);
    EmitVertex();

    gl_Position = VP * vec4(Pos + right * 0.5 + up * -0.5, 1.0);
    TexCoord = vec2(1.0, 0.0);
    EmitVertex();

    gl_Position = VP * vec4(Pos + right * -0.5 + up * 0.5, 1.0);
    TexCoord = vec2(0.0, 1.0);
    EmitVertex();

    gl_Position = VP * vec4(Pos + right * -0.5 + up * -0.5, 1.0);
    TexCoord = vec2(0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}