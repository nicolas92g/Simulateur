#version 330 core
#define MAX_NUMBER_OF_SPOT 20
#define MAX_NUMBER_OF_LIGHT 20

//layout(location = 0) out vec3 FragColor;

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

uniform vec3 viewPos;//camera position
uniform sampler2D diffuse;
uniform sampler2D specular;
uniform sampler2D shadowDepthMap;
uniform sampler2D normalMap;
uniform sampler2D metallic;

uniform float gamma = 1;


in vec2 uv;
in vec3 normals;
in mat3 TBN;

in vec3 fragPos;
in vec4 lightFragPos;

out vec4 FragColor;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec4 lightFragmentPosition);
vec3 CalcSpotLight(SpotLight spotLight, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
float calcShadow(vec4 lightFP, vec3 normal, vec3 lightDir);
vec3 calcNormal();

float shininess = 64;//texture(metallic, uv).r * 64;

void main()
{
    vec3 norm = calcNormal();
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 color = vec3(0, 0, 0);
    
    //spots calculation
    for (int i = 0; ((i < int(numberOfSpot)) && (i < MAX_NUMBER_OF_SPOT)); i++) {
        color += CalcSpotLight(spotLight[i], norm, fragPos, viewDir);
    }
    //points light calculation
    for (int i = 0; ((i < int(numberOfLight)) && (i < MAX_NUMBER_OF_LIGHT)) ; i++) {
        color += CalcLight(light[i], norm, fragPos, viewDir);
    }
    //dirLight calculation
    color += CalcDirLight(dirLight, norm, viewDir, lightFragPos);

    //gamma correction
    color = pow(color, vec3(1.0/gamma));

    FragColor.rgb = color;
    FragColor.a = texture(diffuse, uv).a;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec4 lightFP)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(diffuse, uv));
    vec3 diffuse = light.diffuse * diff * vec3(texture(diffuse, uv));
    vec3 specular = light.specular * spec * vec3(texture(specular, uv));
    float shadow = calcShadow(lightFP, normal, lightDir);
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 CalcLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
        light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(texture(diffuse, uv));
    vec3 diffuse = light.diffuse * diff * vec3(texture(diffuse, uv));
    vec3 specular = light.specular * spec * vec3(texture(specular, uv));
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight spotLight, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(spotLight.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float distance = length(spotLight.position - fragPos);
    float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance +
        spotLight.quadratic * (distance * distance));
    //spot light
    float theta = dot(lightDir, normalize(-spotLight.direction));
    float epsilon = spotLight.cutOff - spotLight.outerCutOff;
    float spot =  clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = spotLight.ambient * vec3(texture(diffuse, uv));
    vec3 diffuse = spotLight.diffuse * diff * vec3(texture(diffuse, uv));
    vec3 specular = spotLight.specular * spec * vec3(texture(specular, uv)); 

    ambient *= attenuation;
    diffuse *= attenuation * spot;
    specular *= attenuation * spot;
    return (ambient + diffuse + specular);
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

    return normal;
}