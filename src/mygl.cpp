#include "game.h"
#include "mygl.h"
#include "shader.h"
#include "maths.h"
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "mygltest.cpp"
//#include "mygltestmesh.cpp"
#include "mygltesttext.cpp"

struct Vertex{
    v3 position;
    v3 normal;
    v2 texture_coords;
};

enum TextureType{
    TextureType_DIFFUSE,
    TextureType_SPECULAR,
};

struct Texture{
    GLuint id;
    TextureType type;
};

struct Material{
    GLuint diffuse;
    GLuint specular;
};

struct Mesh{
    Material material;
	Texture texture;

    u32 vertex_count;
    Vertex* vertices;

    u32 index_count;
    u32* indices;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint shader;
};

struct Entity{
	Mesh* mesh;
	v3 world_p;
};


struct Model{
    u32 mesh_count;
    Mesh** meshes;
};

Mesh* MakeCube(){
     float cube_vertices[] = {
         // positions // normals // texture coords
         -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
         0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
         0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
         -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
         -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
         0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
         0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
         -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
         -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
         -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
         -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
         -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
         0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
         -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
     };
	Mesh* mesh = (Mesh*)malloc(sizeof(Mesh));
	memset(mesh, 0, sizeof(Mesh));
	mesh->vertices = (Vertex*)malloc(sizeof(Vertex) * 36);
	memcpy(mesh->vertices, cube_vertices, sizeof(Vertex) * 36);
	mesh->vertex_count = 36;

	ShaderCreate("c:/code/fps/src/shaders/cube_vertex.hlsl", "c:/code/fps/src/shaders/cube_fragment.hlsl", &mesh->shader);

	mesh->texture.id = LoadTexture("c:/code/FPS/src/test_background.bmp");
	
	glGenVertexArrays(1, &(mesh->vao));
	glGenBuffers(1, &(mesh->vbo));
	return mesh;
}

void DrawMesh(Entity e, HWND window){

    glBindVertexArray(e.mesh->vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, e.mesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(e.mesh->indices[0]) * e.mesh->index_count, e.mesh->indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, e.mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(e.mesh->vertices[0]) * e.mesh->vertex_count, e.mesh->vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    static r32 rot = 45;

    mat4 scale = CreateScaleMatrix({1.0f, 1.0f, 1.0f});
    mat4 rotation = CreateRotationMatrix(Normalize({1.0f, 1.0f, 1.0f}), rot);
    mat4 translation = CreateTranslationMatrix({e.world_p.x, e.world_p.y, e.world_p.z});
    mat4 model_mat = scale * rotation * translation;

    mat4 projection = CreatePerspectiveMatrix(ToRadians(90.0f), 0.01f, 1000.0f, 1280.0f, 720.0f);

    mat4 view = LookAt(game_state->camera_p, game_state->camera_dir + game_state->camera_p);

    ShaderSetUniform(e.mesh->shader, "model", model_mat);
    ShaderSetUniform(e.mesh->shader, "view", view);
    ShaderSetUniform(e.mesh->shader, "projection", projection);

//	glBindTexture(GL_TEXTURE_2D, e.mesh->texture.id);
    glBindVertexArray(e.mesh->vao);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}

void GenerateWorld(Entity* world){
	u32 width = 10;
	u32 height = 10;
	u32 depth = 10;

	for (u32 z = 0; z < depth; z++) {
		for (u32 y = 0; y < height; y++) {
			for (u32 x = 0; x < width; x++) {
				world[z*height*width + y * width + x].mesh = MakeCube();
				world[z*height*width + y * width + x].world_p = { (r32)x, (r32)y, (r32)z };
			}
		}
	}
}

void RenderGame(HWND window, IO* io_in, Memory memory, r32 frame_delta, Game_Input game_input, RECT client_rect){

	persist b32 init = 0;
	persist Entity world[1000];
	if(!init){
		init = 1;
		game_state = (Game_State*)memory.memory;
		SetMemoryArena({&(game_state->d_memory), memory.size - (u32)sizeof(Game_State)});
		game_state->camera_dir = {0, 0, 1};
		io = io_in;
		InitOpenGL(window);
		GenerateWorld(world);
	} 

     v2 new_dimensions;
     new_dimensions.x = (r32)client_rect.right - client_rect.left;
     new_dimensions.y = (r32)client_rect.bottom - client_rect.top;
     if(new_dimensions != game_state->client_dimensions){
		glViewport(0, 0, (GLuint)new_dimensions.x, (GLuint)new_dimensions.y);
		game_state->client_dimensions = new_dimensions;
	}

	v3 normalised_movement = {0};

	r32 movement_speed = 2;
	if(game_input.move_forward.is_down){
		normalised_movement += game_state->camera_dir;
	}
	if(game_input.move_back.is_down){
		normalised_movement -= game_state->camera_dir;
	}
	if(game_input.move_right.is_down){
		normalised_movement += Normalize(Cross(v3_up, game_state->camera_dir));
	}
	if(game_input.move_left.is_down){
		normalised_movement -= Normalize(Cross(v3_up, game_state->camera_dir));
	}
	if(game_input.move_down.is_down){
		normalised_movement.y = -1;
	}
	if(game_input.move_up.is_down){
		normalised_movement.y = 1;
	} 
	
	r32 sensitivity = 0.4f;
	if(game_input.mouse_diff.x > 100 || game_input.mouse_diff.y > 100){}
	else{
		if(game_input.mouse_diff.x != 0){
			game_state->camera_yaw += (game_input.mouse_diff.x * sensitivity);
		}
	
		if(game_input.mouse_diff.y != 0){
			game_state->camera_pitch += (game_input.mouse_diff.y * sensitivity);
			if(game_state->camera_pitch > 90){
				game_state->camera_pitch = 90;
			}
			if( game_state->camera_pitch < -90){
				game_state->camera_pitch = -90;
			}
		}
	
		r32 cos_yaw = cos(ToRadians(game_state->camera_yaw));
		r32 sin_yaw = sin(ToRadians(game_state->camera_yaw));
		r32 cos_pitch = cos(ToRadians(game_state->camera_pitch));
		r32 sin_pitch = sin(ToRadians(game_state->camera_pitch));
		game_state->camera_dir.x = cos_yaw * cos_pitch;
		game_state->camera_dir.z = sin_yaw * cos_pitch;
		game_state->camera_dir.y = sin_pitch;
		game_state->camera_dir = Normalize(game_state->camera_dir);
	}
	
	normalised_movement = Normalize(normalised_movement) * movement_speed * frame_delta;
	game_state->camera_p += normalised_movement;
	
	persist Mesh* mesh = MakeCube();
	wglSwapInterval(1);

	glPolygonMode(GL_FRONT_AND_BACK,GL_TRIANGLES);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glClearColor(0.4f, 0.6f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

//	DrawMesh(world[0], window);
	for(int i = 0; i < 1000; i ++){
		DrawMesh(world[i], window);
	}
	
	HDC device_context = GetDC(window);
	SwapBuffers(device_context);
	ReleaseDC(window, device_context);
}
