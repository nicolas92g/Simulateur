#version 330 core
#define PI 3.141592
#define MAX_NUMBER_OF_SPOT 20
#define MAX_NUMBER_OF_LIGHT 20

//OBJECTS 
struct SpotLight {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform uint numberOfSpot;
uniform uint numberOfLight;

uniform SpotLight spotLight[MAX_NUMBER_OF_SPOT];
uniform Light light[MAX_NUMBER_OF_LIGHT];
uniform DirLight dirLight;

struct Material{
    vec3 baseColor;
    float metallic;
    float roughness;
    float ao;
    vec3 normal;
    float alpha;
};
Material pbr;

//PROTOTYPES
vec3 CalcDirLight(DirLight light, vec3 F0, vec3 viewDir, vec4 lightFragmentPosition);
vec3 CalcSpotLight(SpotLight spotLight, vec3 F0, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcLight(Light light, vec3 F0, vec3 normal, vec3 fragPos, vec3 viewDir);
float calcShadow(vec4 lightFP, vec3 normal, vec3 lightDir);
vec3 calcNormal(sampler2D tex, float uvFactor);
void calcColor(vec3 N);
Material mixM(Material a, Material b, float factor);

float DistributionGGX(vec3 N, vec3 H, float a);
float GeometrySchlickGGX(float NdotV, float k);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float k);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

//INPUTS
in vec2 uv;
in vec3 fragPos;
in vec4 lightFragPos;
in vec3 normals;
in mat3 TBN;

out vec4 FragColor;
  
uniform vec3 viewPos;//position of the camera that render with this shader
uniform mat4 environmentRotation;//rotation of the environment map
uniform float ambientStrength;//multiplicator of the ambient light
uniform bool water;
uniform float time;
uniform float seaLevel;

uniform sampler2D diffuse;//this name is just to be the same as the non-pbr shader, it represent albedo or base-color
uniform sampler2D specular;//how metallic is the fragment
uniform sampler2D roughness;//how rough is the fragment
uniform sampler2D ao;//ambient occlusion map
uniform sampler2D normalMap;// tangent normal map
uniform sampler2D waterBump;//water normal map
uniform sampler2D refractionColor;//refraction color map
uniform sampler2D refractionDepth;//refraction depth map

uniform samplerCube irradianceMap;// irrandiance map to determine diffuse light of a fragment
uniform samplerCube prefilterMap;//environment map pre-filtered with 5 mip-map of different roughness
uniform sampler2D   brdfLUT;//brdf Texture (Weird stuff i am too lazy to learn what it is but i know we need this in the pbr calculation)
uniform sampler2D   shadowDepthMap;//shadow depth map for the dir light

Material snow;
Material grass;
Material rock;
Material sand;

#define GROUND_SIZE 0.01
#define FOG_DISTANCE 3300

#define GRASS_MAX 50
#define GRASS_TRANSITION 50

#define ROCK_MAX 200
#define ROCK_TRANSITION 250

void main(){
    
    //fill material with texture
    pbr.baseColor = vec3(1);
    pbr.alpha = 1;//texture(diffuse, uv).a;
    pbr.metallic = 0;//texture(specular, uv).r;
    pbr.roughness = 1;//texture(roughness, uv).r;
    pbr.normal = calcNormal(normalMap, GROUND_SIZE);

    sand.metallic = 0.1;
    sand.roughness = 0.2;  
    sand.normal = calcNormal(normalMap, 0.02);
    sand.baseColor = vec3(0.9, 0.7, 0.15);

    rock.metallic = 1;
    rock.roughness = 0.9;
    rock.normal = pbr.normal;
    rock.baseColor = vec3(0.65, 0.45, 0.4);
  
    snow.metallic = 0.01;
    snow.roughness = 0.01;
    snow.normal = calcNormal(normalMap, 0.1);
    snow.baseColor = vec3(1);

    grass.metallic = 0.01;
    grass.roughness = 0.8;
    grass.normal = calcNormal(normalMap, 0.05);
    grass.baseColor = vec3(0.13, 0.35, 0.02);

    calcColor(pbr.normal); 

    pbr.ao = 1;

   
    




    //fragment to eye of the camera vector
    vec3 V = normalize(viewPos - fragPos);
    vec3 R = reflect(-V, pbr.normal);

    //surface reflection at zero incidence
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, pbr.baseColor, pbr.metallic);
	           
   
    vec3 Lo = vec3(0);

    //points lights
    for (int i = 0; ((i < int(numberOfLight)) && (i < MAX_NUMBER_OF_LIGHT)) ; i++)
    {           
        Lo += CalcLight(light[i], F0, pbr.normal, fragPos, V);
    }
//
//    //spots lights
//    for (int i = 0; ((i < int(numberOfSpot)) && (i < MAX_NUMBER_OF_LIGHT)) ; i++)
//    {           
//        //Lo += CalcSpotLight(spotLight[i], F0, pbr.normal, fragPos, V);
//    }

    //directional light
    Lo += CalcDirLight(dirLight, F0, V, lightFragPos);

    vec3 F = fresnelSchlickRoughness(max(dot(pbr.normal, V), 0), F0, pbr.roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - pbr.metallic;

    vec3 irradiance = texture(irradianceMap, pbr.normal).rgb;
    vec3 diffuseValue = irradiance * pbr.baseColor;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, vec3(vec4(R,1) * environmentRotation),  pbr.roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(pbr.normal, V), 0.0), pbr.roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuseValue + specular) * pbr.ao;

    vec3 color = (ambient * ambientStrength) + Lo;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    float dst = distance(vec3(viewPos.x, 0, viewPos.z), fragPos);
        if(dst > FOG_DISTANCE){
        pbr.alpha -= (dst - FOG_DISTANCE) * 0.002;
    }
   
    FragColor = vec4(color, pbr.alpha);

    if(viewPos.y < seaLevel){
        
        float dst = distance(fragPos, viewPos);
        FragColor.rgb = mix(FragColor.rgb, vec3(0.1, 0.3, 0.9), dst * 0.05 + 0.2);
    }
}

void calcColor(vec3 N){
    if(water){
        vec3 normal = texture(waterBump, uv * 32 + (time + cos(time) * 0.1) * 0.02 * vec2(-0.4,1)).rgb;
        vec3 normal2 = texture(waterBump, (vec2(1, 1) - uv) * 32 + (time) * 0.01 * vec2(-1,0.2)).rgb;

        normal = mix(normal, normal2, 0.5);
        normal = (normal * 2.0 - 1.0);
        normal = normalize(TBN * normal);
        
    
        pbr.roughness = 0.05;
        pbr.metallic = 1;
        pbr.normal = normal;
        pbr.alpha = 0.5;

        pbr.alpha = min(mix(pbr.alpha, 1, distance(fragPos, viewPos) * 0.02), 1);
        
        pbr.baseColor = vec3(0.1, 0.4, 0.9);
        return;
    }

    float angle = abs(dot(vec3(0,1,0), N));
    //angle = angle * 5 - 4;//range
    //angle = max(min(1, 1 - (angle - 0.6) * 2.5), 0);

    pbr = grass;   


    float offset = cos((fragPos.x + (fragPos.z)) * 0.005 - round((fragPos.x + fragPos.z) * 0.01));
    offset = abs(cos(offset * 15) + 0.7) * 3;

    

    if(fragPos.y < 20 + seaLevel + offset && fragPos.y > seaLevel + offset){
        pbr = mixM(sand, pbr, (fragPos.y - seaLevel + offset) * 0.045);
    }
    if(fragPos.y < seaLevel + offset){
        pbr = mixM(rock, sand, max(fragPos.y, 0));
    }

    offset = offset * -3.3 ;

    if(fragPos.y > GRASS_MAX + offset){
        pbr = mixM(pbr, rock, (fragPos.y - GRASS_MAX - offset) / GRASS_TRANSITION );
    }
    if(fragPos.y > GRASS_MAX + GRASS_TRANSITION + offset){
        pbr = rock;

    }

    offset *= -0.5;

    if(fragPos.y > ROCK_MAX + offset){ 
        pbr = mixM(pbr, snow, (fragPos.y - ROCK_MAX - offset) / ROCK_TRANSITION);   
    }
    if(fragPos.y > ROCK_MAX + ROCK_TRANSITION + offset){
        pbr = snow;
    }
    float dst = distance(viewPos, fragPos);
    if(dst < 500){
        if (angle < 0.3 && fragPos.y > ROCK_MAX + offset){
            pbr = mixM(pbr, rock, 1 - angle - max((dst - 400), 0) * 0.01);
        }
    }
    
    pbr.alpha = 1;
       
}

vec3 CalcDirLight(DirLight light, vec3 F0, vec3 viewDir, vec4 lightFragmentPosition){
    vec3 LightDir = normalize(-light.direction);
    // calculate per-light radiance
    vec3 H = normalize(viewDir + LightDir);
    vec3 radiance = light.diffuse;
    
    // cook-torrance brdf
    float NDF = DistributionGGX(pbr.normal, H, pbr.roughness);        
    float G   = GeometrySmith(pbr.normal, viewDir, LightDir, pbr.roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);       
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - pbr.metallic;	  
    
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(pbr.normal, viewDir), 0.0) * max(dot(pbr.normal, LightDir), 0.0);
    vec3 specular     = numerator / max(denominator, 0.001);
        
    float NdotL = max(dot(pbr.normal, LightDir), 0.0);  
    
    float shadow = calcShadow(lightFragPos, pbr.normal, LightDir);

    return (1.0 - shadow) * ((kD * pbr.baseColor / PI + specular) * radiance * NdotL);
}
vec3 CalcLight(Light light, vec3 F0, vec3 normal, vec3 fragPos, vec3 viewDir){
    // calculate per-light radiance
        vec3 L = normalize(light.position - fragPos);
        if(distance(light.position, fragPos) > 50) return vec3(0);
        vec3 H = normalize(viewDir + L);
        float distance    = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        vec3 radiance     = light.diffuse * attenuation;
        
        // cook-torrance brdf
        float NDF = DistributionGGX(pbr.normal, H, pbr.roughness);
        float G   = GeometrySmith(pbr.normal, viewDir, L, pbr.roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);       
        
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - pbr.metallic;	  
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(pbr.normal, viewDir), 0.0) * max(dot(pbr.normal, L), 0.0);
        vec3 specularColor     = numerator / max(denominator, 0.001) * attenuation;
            
        // add to outgoing radiance Lo
        float NdotL = max(dot(pbr.normal, L), 0.0);                
        return (kD * pbr.baseColor / PI + specularColor) * radiance * NdotL;
}

vec3 CalcSpotLight(SpotLight spotLight, vec3 F0, vec3 normal, vec3 fragPos, vec3 viewDir){
    //create a point light with the spot values
    Light spot; spot.position = spotLight.position; spot.diffuse = spotLight.diffuse; 
    spot.constant = spotLight.constant; spot.linear = spotLight.linear; spot.quadratic = spotLight.quadratic;
    //calculate this point light 
    vec3 pointLight = CalcLight(spot, F0, normal, fragPos, viewDir);

    //light direction
    vec3 L = normalize(spotLight.position - fragPos);

    //determine spot 
    float theta = dot(L, normalize(-spotLight.direction));
    float epsilon = spotLight.cutOff - spotLight.outerCutOff;
    float spotStrength =  clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
    
    //return the spot color
    return pointLight * spotStrength;
}

float DistributionGGX(vec3 N, vec3 H, float a)
{
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float nom    = a2;
    float denom  = (NdotH2 * (a2 - 1.0) + 1.0);
    denom        = PI * denom * denom;
	
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float k)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return nom / denom;
}
  
float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}  

vec3 calcNormal(sampler2D tex, float uvFactor){
    vec3 normal = texture(tex, uv * uvFactor).rgb;

    //test if normal map is empty or doesn't exist
    if (normal == vec3(0)){
        normal = normalize(normals);
    }
    else{
        normal = (normal * 2.0 - 1.0);
        normal = normalize(TBN * normal);
    }
    //      return normals;
    return normal;
}

float calcShadow(vec4 lightFP, vec3 normal, vec3 lightDir){
    // perform perspective divide
    vec3 projCoords = lightFP.xyz / lightFP.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    //test if the fragment is in the texture and can be determine
    if(projCoords.z > 1.0)
        return 0.0;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowDepthMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    float bias = 0.004;//max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowDepthMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowDepthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            if (currentDepth - bias > pcfDepth){
                shadow += 1.0 / 9.0;
            }
        }    
    }
    return shadow;
}
Material mixM(Material a, Material b, float factor){
    factor = max(min(factor, 1), 0);
    Material ret;
    ret.baseColor = mix(a.baseColor, b.baseColor, factor);
    ret.metallic = mix(a.metallic, b.metallic, factor);
    ret.roughness = mix(a.roughness, b.roughness, factor);
    ret.alpha = mix(a.alpha, b.alpha, factor);
    ret.ao = 1;

    ret.normal = mix(a.normal, b.normal, factor);

    return ret;
}
