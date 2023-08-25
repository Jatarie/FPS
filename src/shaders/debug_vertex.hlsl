#version 330 core

layout (location = 0) in vec3 v_pos;
uniform mat4 u_view;
uniform mat4 u_projection;


void main(){
//	mat4 model = mat4(1.0, 0.0, 0.0, 0.0,  // 1. column
//	                  0.0, 1.0, 0.0, 0.0,  // 2. column
//	                  0.0, 0.0, 1.0, 0.0,  // 3. column
//	                  v_pos.x, v_pos.y, v_pos.z, 1.0); // 4. column


    gl_Position = u_projection * u_view * vec4(v_pos, 1);
    
}