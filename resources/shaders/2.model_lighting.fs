#version 330 core
out vec4 FragColor;

struct PointLight {
    vec3 position;

    vec3 specular;
    vec3 diffuse;
    vec3 ambient;

    float constant;
    float linear;
    float quadratic;
};

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;

    float shininess;
};

struct DirLight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


struct SpotLight{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

};

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform PointLight pointLight;
uniform Material material;
uniform DirLight directional;
uniform SpotLight spotlight;
uniform bool blinn;


uniform vec3 viewPosition;
// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    //Blinn-Phong
    if(blinn){
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir),0.0), material.shininess);
    }
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords).xxx);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){

    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = 0.0f;
    //Blinn-Phong
    if(blinn){
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir),0.0), material.shininess);
    }else{
        spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
    }

    vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;
    vec3 specular = light.specular * spec * texture(material.texture_specular1, TexCoords).rgb;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
    //ambient
    vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;
    //diffuse
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir, normal),0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;
    //specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0f;

    //Blinn-Phong
    if(blinn){
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir),0.0), material.shininess);
    }else{
        spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
    }

    vec3 specular = light.specular * spec * texture(material.texture_specular1, TexCoords).xyz;


    //attenuation
    float d = length(light.position - fragPos);
    float att = 1.0/(d*d);
    ambient *= att;
    diffuse *= att;
    specular *= att;
    //spotlight
    float theta = dot(-lightDir, normalize(light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff)/epsilon, 0.0, 1.0);
    ambient *= intensity;
    diffuse *= intensity;
    specular *= intensity;

    return (ambient + diffuse + specular);
}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);
    vec3 result = CalcPointLight(pointLight, normal, FragPos, viewDir);
    result += CalcDirLight(directional, normal, viewDir);
    result += CalcSpotLight(spotlight, normal, FragPos, viewDir);
    FragColor = vec4(result, 1.0);
}