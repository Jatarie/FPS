#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 light_pos;
out vec3 pos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 in_light_pos;
uniform vec3 in_world_pos;


void main(){
    // pos = aPos;
    pos = in_world_pos;

    TexCoord = aTexCoord;
    light_pos = in_light_pos;

    gl_Position = projection * view* model * vec4(aPos, 1);
    
}