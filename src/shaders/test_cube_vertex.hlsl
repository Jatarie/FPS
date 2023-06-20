#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec4 pos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main(){

    Normal = mat3(transpose(inverse(model))) * aNormal;
    // Normal = aNormal;

    TexCoord = aTexCoord;
    vec4 world_pos = projection * view* model * vec4(aPos, 1);
    pos = model * vec4(aPos, 1);

    gl_Position = world_pos;
}