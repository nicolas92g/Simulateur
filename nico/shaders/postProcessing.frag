#version 330 core
#define NUM_STEP 10

out vec4 FragColor;
in vec2 uv;

//inputs
uniform float zNear;
uniform float zFar;
uniform vec3 viewPos;
uniform vec3 boxPos;
uniform vec3 boxScale;
uniform vec3 look;

//textures
uniform sampler2D rendering;
uniform sampler2D depthMap;
uniform sampler2D ppDepthMap;
uniform sampler2D fragDirMap;
uniform sampler3D noise;

//parameters
const float densityThreshold = 0.01;
const float densityMultiplier = 2000;

struct Ray{
    vec3 dir;
    vec3 o;
};
struct Hit{
    float tmin;
    float tmax;
};

bool BBoxIntersect(const vec3 boxMin, const vec3 boxMax, const Ray r, out Hit hit) {
    vec3 tbot = -r.dir * (boxMin - r.o);
    vec3 ttop = -r.dir * (boxMax - r.o);
    vec3 tmin = min(ttop, tbot);
    vec3 tmax = max(ttop, tbot);
    vec2 t = max(tmin.xx, tmin.yz);
    float t0 = max(t.x, t.y);
    t = min(tmax.xx, tmax.yz);
    float t1 = min(t.x, t.y);
    hit.tmin = t0;
    hit.tmax = t1;
    return t1 > max(t0, 0.0);
}


vec2 rayBoxDistance(vec3 boundsMin, vec3 boundsMax, vec3 rayOrigin, vec3 rayDir);
float sampleDensity(vec3 position);
float getRealZ(float depthBufferValue);
    
void main()
{
    //get the distance from the camera of the fragment
    float ppDepth = texture(ppDepthMap, uv).r;
    float RDepth = texture(depthMap, uv).r;
    vec3 V = texture(fragDirMap, uv).rgb;

    FragColor = texture(rendering, uv);

//    float depth = getRealZ(RDepth);
//
//    if((ppDepth < RDepth)){
//        FragColor.rgb = vec3(1, 0.6, 0.8);
//    }
//
//    vec2 dst = rayBoxDistance(boxPos - boxScale / 2, boxPos + boxScale / 2, viewPos, V);
//    
//    float dstLimit = min(depth - dst.x, dst.y);
//    float dstTravelled = 0;
//    float stepSize = dst.y / NUM_STEP;
//
//    float totalDensity = 0;
//    while(dstTravelled < dstLimit){
//        vec3 rayPos = viewPos + V * (dstTravelled + dst.x);
//        totalDensity += sampleDensity(rayPos) * stepSize;
//    }
//    float transmittance = exp(-totalDensity);
//
    FragColor.a = 1;
}   
//x = dst to box , y = dst inside box
//return the distance to the box then the distance that the ray travel inside 
//the box (if the ray miss the box, dstInsideBox will return 0 (y var))
vec2 rayBoxDistance(vec3 boundsMin, vec3 boundsMax, vec3 rayOrigin, vec3 rayDir){
    vec3 t0 = (boundsMin - rayOrigin) / rayDir;
    vec3 t1 = (boundsMax - rayOrigin) / rayDir;
    vec3 tmin = min(t0, t1);
    vec3 tmax = max(t0, t1);

    float dstA = max(max(tmin.x, tmin.y), tmin.z);
    float dstB = min(tmax.x, min(tmax.y, tmax.z));

    float dstToBox = max(0, dstA);
    float dstInsideBox = max(0, dstB - dstToBox);

    return vec2(dstToBox, dstInsideBox);
}

float sampleDensity(vec3 position){
    return 10;//DENSITY DELETE
    vec3 uvw = position * boxScale * 0.001+ boxPos * 0.01;
    float density = texture(noise, uvw).r;
    return max(0, density - densityThreshold) * densityMultiplier;
}

float getRealZ(float depthBufferValue){
    depthBufferValue = 2.0 * depthBufferValue - 1.0;
    float zLinear = 2.0 * zNear * zFar / (zFar + zNear - depthBufferValue * (zFar - zNear));
    return zLinear;
}