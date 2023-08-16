#include "game.h"
#include "mygl.h"
#include "shader.h"
#include "maths.h"
#include <stdio.h>
#include "datastructures.h"

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

};

struct Entity{
	Mesh* mesh;
	v3 world_p;
};


struct Model{
    u32 mesh_count;
    Mesh** meshes;
};

static float cube_vertices[] = {
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

struct VertexDataFormat{
	u32 vertex_size_bytes;
	u32 attrib_count;
	u32* attrib_sizes;
};

struct RenderGroup{
	GLuint vao, vbo, shader_program;
	Array vertex_data;
	u32 vertex_data_size;
	u32 vertex_count;
	VertexDataFormat format;
};

Mesh* MakeCube(){
	Mesh* mesh = (Mesh*)malloc(sizeof(Mesh));
	memset(mesh, 0, sizeof(Mesh));
	mesh->vertices = (Vertex*)malloc(sizeof(Vertex) * 36);
	memcpy(mesh->vertices, cube_vertices, sizeof(Vertex) * 36);
	mesh->vertex_count = 36;
	mesh->texture.id = LoadTexture("c:/code/FPS/src/test_background.bmp");
	
	return mesh;
}

void DrawVertices(RenderGroup *group){
    glBindVertexArray(group->vao);
    glBindBuffer(GL_ARRAY_BUFFER, group->vbo);
    glBufferData(GL_ARRAY_BUFFER, group->vertex_data_size, group->vertex_data.data, GL_STATIC_DRAW);

	u32 offset = 0;
	for(int i = 0 ; i < group->format.attrib_count; i++){
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, group->format.attrib_sizes[i], GL_FLOAT, GL_FALSE, group->format.vertex_size_bytes, (void*)(sizeof(r32) * offset));
		offset += group->format.attrib_sizes[i];
	}

    glBindBuffer(GL_ARRAY_BUFFER, 0);

//    mat4 scale = CreateScaleMatrix({1.0f, 1.0f, 1.0f});
//    mat4 rotation = CreateRotationMatrix(Normalize({1.0f, 1.0f, 1.0f}), rot);
//    mat4 translation = CreateTranslationMatrix({0, 0, 0});
//    mat4 model_mat = scale * rotation * translation;

    mat4 projection = CreatePerspectiveMatrix(ToRadians(90.0f), 0.01f, 1000.0f, 1280.0f, 720.0f);
    mat4 view = LookAt(game_state->camera_p, game_state->camera_dir + game_state->camera_p);

	ShaderSetUniform(group->shader_program, "view", view);
    ShaderSetUniform(group->shader_program, "projection", projection);

	glDrawArrays(GL_TRIANGLES, 0, group->vertex_count);

	for(int i = 0 ; i < group->format.attrib_count; i ++){
		glDisableVertexAttribArray(i);
	}
    glBindVertexArray(0);
}

void PushVertexData(v3* data, RenderGroup* group){
	group->vertex_data.Add((r32*)data, sizeof(v3));
	group->vertex_data_size += sizeof(v3);
}

void PushVertexData(v2* data, RenderGroup* group){
	group->vertex_data.Add((r32*)data, sizeof(v2));
	group->vertex_data_size += sizeof(v2);
}

void PushMesh(Entity e, RenderGroup *group){
	for(int i = 0 ; i < e.mesh->vertex_count; i ++){
		PushVertexData(&(e.mesh->vertices[i].position), group);
		PushVertexData(&(e.mesh->vertices[i].normal), group);
		PushVertexData(&(e.mesh->vertices[i].texture_coords), group);
		PushVertexData(&(e.world_p), group);
		group->vertex_count += 1;
	}
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

RenderGroup* CreateRenderGroup(u32* attribs, u32 attrib_count){
	RenderGroup *group = (RenderGroup*)malloc(sizeof(RenderGroup));
	group->vertex_data = Array(1024);
	group->format.attrib_count = attrib_count;
	group->format.attrib_sizes = (u32*)malloc(attrib_count);
	memcpy(group->format.attrib_sizes, attribs, attrib_count*sizeof(u32));
	for(int i = 0 ; i < attrib_count; i ++){
		group->format.vertex_size_bytes += attribs[i] * sizeof(r32);
	}
	return group;
}

void RenderGame(HWND window, IO* io_in, Memory memory, r32 frame_delta, Game_Input game_input, RECT client_rect) {
	persist b32 init = 0;
	persist Entity world[1000];
	persist RenderGroup *group;

	if (!init) {
		init = 1;
		game_state = (Game_State*)memory.memory;
		SetMemoryArena( { &(game_state->d_memory), memory.size - (u32)sizeof(Game_State) });
		game_state->camera_dir = { 0, 0, 1 };
		io = io_in;
		InitOpenGL(window);

		GenerateWorld(world);

		u32 attribs[] = { 3, 3, 2, 3 };
		group = CreateRenderGroup(attribs, ARRAYCOUNT(attribs));
		ShaderCreate("c:/code/fps/src/shaders/cube_vertex.hlsl", "c:/code/fps/src/shaders/cube_fragment.hlsl", &(group->shader_program));

		
		glGenVertexArrays(1, &(group->vao));
		glGenBuffers(1, &(group->vbo));
	}

	group->vertex_data_size = 0;
	group->vertex_data.count = 0;
	group->vertex_count = 0;

	v2 new_dimensions;
	new_dimensions.x = (r32)client_rect.right - client_rect.left;
	new_dimensions.y = (r32)client_rect.bottom - client_rect.top;
	if (new_dimensions != game_state->client_dimensions) {
		glViewport(0, 0, (GLuint)new_dimensions.x, (GLuint)new_dimensions.y);
		game_state->client_dimensions = new_dimensions;
	}

	v3 normalised_movement = { 0 };

	r32 movement_speed = 2;
	if (game_input.move_forward.is_down) {
		normalised_movement += game_state->camera_dir;
	}
	if (game_input.move_back.is_down) {
		normalised_movement -= game_state->camera_dir;
	}
	if (game_input.move_right.is_down) {
		normalised_movement += Normalize(Cross(v3_up, game_state->camera_dir));
	}
	if (game_input.move_left.is_down) {
		normalised_movement -= Normalize(Cross(v3_up, game_state->camera_dir));
	}
	if (game_input.move_down.is_down) {
		normalised_movement.y = -1;
	}
	if (game_input.move_up.is_down) {
		normalised_movement.y = 1;
	}
	
	r32 sensitivity = 0.4f;
	if (game_input.mouse_diff.x > 100 || game_input.mouse_diff.y > 100){}
	else {
		if (game_input.mouse_diff.x != 0) {
			game_state->camera_yaw += (game_input.mouse_diff.x * sensitivity);
		}
	
		if (game_input.mouse_diff.y != 0) {
			game_state->camera_pitch += (game_input.mouse_diff.y * sensitivity);
			if (game_state->camera_pitch > 90) {
				game_state->camera_pitch = 90;
			}
			if (game_state->camera_pitch < -90) {
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

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.4f, 0.6f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	for(int i = 0; i < 1000; i ++){
		PushMesh(world[i], group);
	}
//	PushMesh(world[1], group);
	DrawVertices(group);

	
	HDC device_context = GetDC(window);
	SwapBuffers(device_context);
	ReleaseDC(window, device_context);
}
