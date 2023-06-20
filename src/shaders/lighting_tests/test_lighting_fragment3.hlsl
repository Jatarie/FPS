#version 330 core

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};

struct Light{
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

// uniform sampler2D ourTexture;
uniform vec3 camera_pos;
uniform Material material;
uniform Light light;

void main(){
    vec3 lightDir = normalize(light.position - world_pos);
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutoff - light.outercutoff;
    float intensity = clamp((theta - light.outercutoff) / epsilon, 0.0, 1.0);

    if(theta > light.outercutoff) {
        // do lighting calculations
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
        
        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoord).rgb;  
        
        vec3 viewDir = normalize(camera_pos - world_pos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * texture(material.specular, TexCoord).rgb;  
        
        float distance = length(light.position - world_pos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        diffuse *= attenuation;
        diffuse *= intensity;

        specular *= attenuation;
        specular *= intensity;

        vec3 result = ambient + diffuse + specular;

        FragColor = vec4(result, 1.0);
    }
    else{
        // use ambient light so scene isn’t black outside the spotlight.
        FragColor = vec4(light.ambient * vec3(texture(material.diffuse, TexCoord)), 1.0);
    } 

}