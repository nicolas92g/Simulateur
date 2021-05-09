#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormals;
layout(location = 2) in vec2 aUv;
layout(location = 3) in vec3 aTangents;
layout(location = 4) in vec3 aBitangents;
layout(location = 5) in mat4 aInstancedModels;

out vec3 normals;
out vec2 uv;
out mat3 TBN;

//this is the position of the fragment in world space
out vec3 fragPos;

//this use for shadows :
out vec4 lightFragPos;
uniform mat4 lightSpaceMatrix;

//this is matrices to transform 3D objects into 2D pixels
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec4 clipPlane;
uniform bool instanced;

void main()
{
    mat4 modelMatrix;
    if(instanced) {
        modelMatrix = aInstancedModels;
    }
    else{
        modelMatrix = model;
    }

    vec4 worldTrans = modelMatrix * vec4(aPos , 1.0);
    
    gl_Position = projection * view * worldTrans;
    gl_ClipDistance[0] = dot(worldTrans, clipPlane);

    normals = aNormals;
    uv = aUv;
    fragPos = vec3(model * vec4(aPos, 1));
    lightFragPos = lightSpaceMatrix * vec4(fragPos, 1) ;

    vec3 T = normalize(vec3(modelMatrix * vec4(aTangents, 0.0)));
    vec3 N = normalize(vec3(modelMatrix * vec4(aNormals, 0.0)));
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    vec3 B = cross(N, T);
    
    TBN = mat3(T, B, N);
}