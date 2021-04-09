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
vec3 calcNormal();

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
  
uniform sampler2D diffuse;//this name is just to be the same as the non-pbr shader, it represent albedo or base-color
uniform sampler2D specular;//how metallic is the fragment
uniform sampler2D roughness;//how rough is the fragment
uniform sampler2D ao;//ambient occlusion map
uniform sampler2D normalMap;// tangent normal map 

uniform samplerCube irradianceMap;// irrandiance map to determine diffuse light of a fragment
uniform samplerCube prefilterMap;//environment map pre-filtered with 5 mip-map with different roughness
uniform sampler2D   brdfLUT;  //brdf Texture (Weird stuff i am too lazy to learn what it is but i know we need this in the specular pbr calculation)
uniform sampler2D   shadowDepthMap;//shadow depth map for the dir light


void main(){
    //fill material with texture
    pbr.baseColor = pow(texture(diffuse, uv).rgb, vec3(2.2));
    pbr.alpha = 1;//texture(diffuse, uv).a;
    pbr.metallic = texture(specular, uv).r;
    pbr.roughness = texture(roughness, uv).r;
    pbr.ao = 1;//texture(ao, uv).r;
    pbr.normal = calcNormal();

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

    //spots lights
    for (int i = 0; ((i < int(numberOfSpot)) && (i < MAX_NUMBER_OF_LIGHT)) ; i++)
    {           
        Lo += CalcSpotLight(spotLight[i], F0, pbr.normal, fragPos, V);
    }

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
   
    FragColor = vec4(color, pbr.alpha);
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
        vec3 specularColor     = numerator / max(denominator, 0.001) ;//* attenuation;
            
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

vec3 calcNormal(){
    vec3 normal = texture(normalMap, uv).rgb;

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
