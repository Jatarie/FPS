#version 330 core

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Directional_Light{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Point_Light{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct Spot_Light{
    vec3 position;
    vec3 direction;

    float cutoff;
    float outercutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

out vec4 FragColor;
in vec2 TexCoord;
in vec3 world_pos;
in vec3 Normal;

uniform vec3 camera_pos;
uniform Material material;
#define NUM_LIGHTS 4
uniform Point_Light point_light[NUM_LIGHTS];
uniform Directional_Light directional_light;
uniform Spot_Light spot_light;

vec3 CalculateDirectionalLight(Directional_Light l){
    vec3 lightDir = normalize(l.direction);

    vec3 ambient = l.ambient * vec3(texture(material.diffuse, TexCoord));
    
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = l.diffuse * diff * texture(material.diffuse, TexCoord).rgb;  
    
    vec3 viewDir = normalize(camera_pos - world_pos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = l.specular * spec * texture(material.specular, TexCoord).rgb;  
    
    vec3 result = ambient + diffuse + specular;

    return result;
}

vec3 CalculatePointLight(Point_Light l){
    vec3 lightDir = normalize(l.position - world_pos);

    vec3 ambient = l.ambient * vec3(texture(material.diffuse, TexCoord));
    
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = l.diffuse * diff * texture(material.diffuse, TexCoord).rgb;  
    
    vec3 viewDir = normalize(camera_pos - world_pos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = l.specular * spec * texture(material.specular, TexCoord).rgb;  
    
    float distance = length(l.position - world_pos);
    float attenuation = 1.0 / (l.constant + l.linear * distance + l.quadratic * (distance * distance));

    diffuse *= attenuation;
    specular *= attenuation;
    ambient *= attenuation;

    vec3 result = ambient + diffuse + specular;

    return result;
}

vec3 CalculateSpotLight(Spot_Light l){
    vec3 lightDir = normalize(l.position - world_pos);
    float theta = dot(lightDir, normalize(-l.direction));
    float epsilon = l.cutoff - l.outercutoff;
    float intensity = clamp((theta - l.outercutoff) / epsilon, 0.0, 1.0);

    if(theta > l.outercutoff) {
        // do lighting calculations
        vec3 ambient = l.ambient * vec3(texture(material.diffuse, TexCoord));
        
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = l.diffuse * diff * texture(material.diffuse, TexCoord).rgb;  
        
        vec3 viewDir = normalize(camera_pos - world_pos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = l.specular * spec * texture(material.specular, TexCoord).rgb;  
        
        float distance = length(l.position - world_pos);
        float attenuation = 1.0 / (l.constant + l.linear * distance + l.quadratic * (distance * distance));

        diffuse *= attenuation;
        diffuse *= intensity;

        specular *= attenuation;
        specular *= intensity;

        vec3 result = ambient + diffuse + specular;

        return result;
    }
    else{
        // use ambient light so scene isn’t black outside the spotlight.
        return l.ambient * vec3(texture(material.diffuse, TexCoord));
    } 
}

vec3 calc(Spot_Light l){
    // ambient
    vec3 ambient = l.ambient * texture(material.diffuse, TexCoord).rgb;
    
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(l.position - world_pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = l.diffuse * diff * texture(material.diffuse, TexCoord).rgb;  
    
    // specular
    vec3 viewDir = normalize(camera_pos - world_pos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = l.specular * spec * texture(material.specular, TexCoord).rgb;  
    
    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-l.direction)); 
    float epsilon = (l.cutoff - l.outercutoff);
    float intensity = clamp((theta - l.outercutoff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;
    
    // attenuation
    float distance    = length(l.position - world_pos);
    float attenuation = 1.0 / (l.constant + l.linear * distance + l.quadratic * (distance * distance));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;   
        
    return ambient + diffuse + specular;
}

void main(){
    vec3 result = vec3(0);
    // result = CalculateDirectionalLight(directional_light);
    for(int i = 0; i < NUM_LIGHTS; i++){
        // result += CalculatePointLight(point_light[i]);
    }
    result += CalculateSpotLight(spot_light);
    FragColor = vec4(result, 1);
}