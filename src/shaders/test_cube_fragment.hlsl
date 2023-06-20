#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 light_pos;
in vec4 pos;
in vec3 Normal;

uniform sampler2D ourTexture;

void main(){
    FragColor = texture(ourTexture, TexCoord);
    // FragColor = vec4((Normal+vec3(1))/2 ,1.0f);
}