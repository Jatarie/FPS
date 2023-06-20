#version 330 core

layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 Texture_Coords;

out vec2 texture_coords;
out vec3 normal;
out vec3 frag_world_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main(){
    normal = mat3(transpose(inverse(model))) * Normal;

    texture_coords = Texture_Coords;
    vec4 pos = projection * view* model * vec4(Pos, 1);
    frag_world_pos = vec3(model * vec4(Pos, 1));

    gl_Position = pos;
}