#version 330 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_normals;
layout (location = 2) in vec2 v_texCoords;
layout (location = 3) in vec3 v_worldPos;
layout (location = 4) in float v_textureId;

out vec2 f_texCoords;
out float f_textureId;
out vec3 f_normals;

uniform mat4 u_view;
uniform mat4 u_projection;


void main(){
	f_texCoords = v_texCoords;
	f_textureId = v_textureId;
	f_normals = v_normals;

	mat4 model = mat4(1.0, 0.0, 0.0, 0.0,  // 1. column
	                  0.0, 1.0, 0.0, 0.0,  // 2. column
	                  0.0, 0.0, 1.0, 0.0,  // 3. column
	                  v_worldPos.x, v_worldPos.y, v_worldPos.z, 1.0); // 4. column


    gl_Position = u_projection * u_view * model * vec4(v_pos, 1);
    
}