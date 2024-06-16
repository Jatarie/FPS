#version 330 core

out vec4 FragColor;

in vec2 f_texCoords;
in vec3 f_color;
//in int f_textureId;
uniform sampler2D u_texture;

void main(){
//	FragColor = vec4(1, 0, 0, 1);
//	FragColor = vec4(f_texCoords, 0, 1);
	vec4 texture_color = texture(u_texture, f_texCoords);
	if(texture_color.a == 0){
		discard;
	}
	FragColor = texture(u_texture, f_texCoords);
}