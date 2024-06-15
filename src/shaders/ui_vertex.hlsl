#version 330 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_color;

uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 f_color;


void main(){
//	mat4 model = mat4(1.0, 0.0, 0.0, 0.0,  // 1. column
//	                  0.0, 1.0, 0.0, 0.0,  // 2. column
//	                  0.0, 0.0, 1.0, 0.0,  // 3. column
//	                  v_pos.x, v_pos.y, v_pos.z, 1.0); // 4. column
	f_color = v_color;


    gl_Position = vec4(v_pos, 1);
    
}