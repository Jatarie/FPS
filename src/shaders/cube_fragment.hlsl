#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 light_pos;
in vec3 pos;

uniform sampler2D ourTexture;

void main(){
//     FragColor = vec4(TexCoord.y, TexCoord.x, 0.0f, 1.0f);
//     FragColor = texture(ourTexture, TexCoord);// * vec4(color, 1.0f);
//     FragColor = texture(ourTexture, TexCoord);// * vec4(color, 1.0f);
//    float distance = sqrt(dot(light_pos, pos));
    // distance = 6.0f;
//    FragColor = vec4(0.1, 0.1, distance/10.0f, 1);// * vec4(color, 1.0f);

     FragColor = vec4(0.8, 0.2, 0.3, 1);
}