#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 light_pos;
in vec3 pos;

uniform sampler2D ourTexture;

void main(){
    FragColor = texture(ourTexture, TexCoord);
}