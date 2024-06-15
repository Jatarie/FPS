#version 330 core

out vec4 FragColor;

//in vec2 f_texCoords;
in vec3 f_color;
//in int f_textureId;
//uniform sampler2D u_textures[2];

void main(){
//	FragColor = vec4(1, 0, 0, 1);
	FragColor = vec4(f_color, 1);
//	FragColor = texture(u_textures[0], f_texCoords);
}