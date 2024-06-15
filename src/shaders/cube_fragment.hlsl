#version 330 core

out vec4 FragColor;

in float f_textureId;
in vec2 f_texCoords;
in vec3 f_normals;

uniform sampler2D u_texture;
//uniform sampler2D u_texture;

void main(){
//	switch(int(f_normals.y)){
//		case 1:{
//			FragColor = texture(u_textures[0], f_texCoords);
//		} break;
//		case -1:{
//			FragColor = texture(u_textures[0], f_texCoords);
//		} break;
//		default:{
//			FragColor = vec4(1, 0, 1, 1);
//		}
//
//	}
	FragColor = texture(u_texture, f_texCoords);
//	FragColor = vec4(0.1, 0.1, 0.1, 1);
//	FragColor = vec4(f_textureId, f_textureId, f_textureId, f_textureId);
//	FragColor = vec4(f_normals, 1);
}