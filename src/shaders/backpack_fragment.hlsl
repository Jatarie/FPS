#version 330 core
out vec4 frag_color;

in vec2 texture_coords;
in vec3 normal;
in vec3 frag_world_pos;

struct Material{
    sampler2D diffuse;
    sampler2D specular;

    float shininess;
};

struct Point_Light{
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

#define NUM_LIGHTS 4
uniform Point_Light point_light[NUM_LIGHTS];
uniform Material material;
uniform vec3 camera_pos;
uniform vec3 camera_dir;

void main(){

    vec3 lightDir = normalize(point_light[0].position - frag_world_pos);
    vec3 viewDir = normalize(camera_pos - frag_world_pos);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 specular = point_light[0].specular * spec * texture(material.specular, texture_coords).r;  
    // vec3 specular = point_light[0].specular * spec;

    vec3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = point_light[0].diffuse * diff * texture(material.diffuse, texture_coords).rgb;  
    // vec3 diffuse = texture(material.diffuse, texture_coords).rgb * point_light[0].diffuse;

    vec3 ambient = point_light[0].ambient * texture(material.diffuse, texture_coords).rgb;


    frag_color = vec4(diffuse + specular + ambient, 1);
}