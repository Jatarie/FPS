#version 330 core

struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;
in vec2 TexCoord;
in vec3 world_pos;
in vec3 Normal;

uniform sampler2D ourTexture;
uniform vec3 camera_pos;
uniform Material material;
uniform Light light;

void main(){
    float PI = 3.14159265f;

    // vec3 light_pos = vec3(0, 0, 0);
    vec3 light_ray_vector = normalize(world_pos - light.position);

    vec3 view_dir = normalize(camera_pos - world_pos);
    vec3 reflect_dir = reflect(light_ray_vector, Normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), material.shininess);
    vec3 specular = spec * light.specular;
    specular = specular * material.specular;

    vec3 v1 = normalize(light_ray_vector);
    vec3 v2 = normalize(Normal);
    float angle = acos(dot(v1, v2));
    angle = angle * (180 / PI);
    angle = angle / 360;
    vec3 diffuse = light.diffuse * angle;
    diffuse = diffuse * material.diffuse;

    vec3 ambient_light_final = light.ambient;
    ambient_light_final = ambient_light_final * material.ambient;
    FragColor = vec4(diffuse + ambient_light_final + specular, 1.0f);


}