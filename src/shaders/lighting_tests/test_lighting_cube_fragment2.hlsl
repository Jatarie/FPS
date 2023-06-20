#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 light_pos;
in vec3 pos;

uniform sampler2D ourTexture;
uniform vec3 light_color;

void main(){
    FragColor = vec4(light_color, 1.0f);
}