#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aWorldPos;

//out vec2 TexCoord;
//out vec3 light_pos;
out vec3 pos;
uniform mat4 view;
uniform mat4 projection;
//uniform vec3 in_light_pos;
uniform vec3 in_world_pos;


void main(){
    pos = in_world_pos;
//    TexCoord = aTexCoord;
//    light_pos = in_light_pos;

	mat4 model = mat4(1.0, 0.0, 0.0, 0.0,  // 1. column
	                  0.0, 1.0, 0.0, 0.0,  // 2. column
	                  0.0, 0.0, 1.0, 0.0,  // 3. column
	                  aWorldPos.x, aWorldPos.y, aWorldPos.z, 1.0); // 4. column


    gl_Position = projection * view * model * vec4(aPos, 1);
    
}