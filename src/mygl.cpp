#include "game.h"
#include "shader.h"
#include "maths.h"
#include <stdio.h>
#include "datastructures.h"
#include "mygl.h"
#include "profile.h"
#include "glm.hpp"
#include "type_ptr.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//#include "mygltest.cpp"
//#include "mygltesttext.cpp"
persist Block world[10000];

Hash_Table* loaded_textures = NULL;
GLuint LoadTexture(char* path_to_image){
	if(!loaded_textures){
		loaded_textures = loaded_textures->Create(100);
	}
	
	GLuint test_if_loaded = loaded_textures->Get(loaded_textures, path_to_image);
	if(test_if_loaded){
		return test_if_loaded;
	}
	
	// Load the image file into memory
	GLuint texture;
	int width, height, nrChannels;
	// stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path_to_image, &width, &height, &nrChannels, 0);
	
	// Generate a texture object
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	GLuint color_format;
	if(nrChannels == 1){
		color_format = GL_RED;
	}
	if(nrChannels == 3){
		color_format = GL_RGB;
	}
	if(nrChannels == 4){
		color_format = GL_RGBA;
	}
	
	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	// Copy the image data to the texture object
	glTexImage2D(GL_TEXTURE_2D, 0, color_format, width, height, 0, color_format, GL_UNSIGNED_BYTE, data);
	
	// Generate mipmaps for the texture
	glGenerateMipmap(GL_TEXTURE_2D);
	
	// Free the image data from memory
	stbi_image_free(data);
	
	loaded_textures->Add(loaded_textures, path_to_image, texture);
	return texture;
}

Rect GetSprite(BlockType type, Faces face){
	Rect r;
	r.bottom_left = v2{block_map[type][face].x / 32.0f + 1/32.0f, block_map[type][face].y / 32.0f + 1/32.0f};
	r.top_right = v2 {block_map[type][face].x /32.0f, block_map[type][face].y / 32.0f};
	return r;
}

global v2 tl = v2 { 0, 1 };
global v2 tr = v2 { 1, 1 };
global v2 bl = v2 { 0, 0 };
global v2 br = v2 { 1, 0 };

void MapTextureCoordinates(Mesh* mesh, u32 i, Rect face){
	if(mesh->vertices[i].texture_coords == bl){
		mesh->vertices[i].texture_coords = face.bottom_left;
	}
	if(mesh->vertices[i].texture_coords == tr){
		mesh->vertices[i].texture_coords = face.top_right;
	}
	if(mesh->vertices[i].texture_coords == tl){
		mesh->vertices[i].texture_coords = v2{face.left, face.top};
	}
	if(mesh->vertices[i].texture_coords == br){
		mesh->vertices[i].texture_coords = v2{face.right, face.bottom};
	}
}

void MakeBlock(Block* block){
	Mesh* mesh = (Mesh*)Malloc(sizeof(Mesh));
	memset(mesh, 0, sizeof(Mesh));
	mesh->vertices = (Vertex*)Malloc(sizeof(Vertex) * 36);
	memcpy(mesh->vertices, cube_vertices, sizeof(Vertex) * 36);
	mesh->vertex_count = 36;
	mesh->texture.id = (u32)(((r32)rand()/RAND_MAX)+0.9f);


	Rect right = GetSprite(block->block_type, Face_Right);
	Rect left = GetSprite(block->block_type, Face_Left);
	Rect front = GetSprite(block->block_type, Face_Front);
	Rect back = GetSprite(block->block_type, Face_Back);
	Rect top = GetSprite(block->block_type, Face_Top);
	Rect bottom = GetSprite(block->block_type, Face_Bottom);


	for(u32 i = 0; i < mesh->vertex_count; i ++){
		switch(int(mesh->vertices[i].normal.x)){
			case 1 : {
				MapTextureCoordinates(mesh, i, right);
			} break;
			case -1 : {
				MapTextureCoordinates(mesh, i, left);
			} break;
		}
		switch(int(mesh->vertices[i].normal.y)){
			case 1 : {
				MapTextureCoordinates(mesh, i, top);
			} break;
			case -1 : {
				MapTextureCoordinates(mesh, i, bottom);
			} break;
		}
		switch(int(mesh->vertices[i].normal.z)){
			case 1 : {
				MapTextureCoordinates(mesh, i, front);
			} break;
			case -1 : {
				MapTextureCoordinates(mesh, i, back);
			} break;
		}
	}
	block->mesh = mesh;
}

v3 raycast;

void Raycast(mat4 view, mat4 projection){
	v4 mouse_coordinates = { 0 };
	mouse_coordinates.x = (r32)game_input.mouse.x / (r32)game_state->client_dimensions.x;
	mouse_coordinates.x *= 2;
	mouse_coordinates.x -= 1;
	mouse_coordinates.y = (r32)game_input.mouse.y / (r32)game_state->client_dimensions.y;
	mouse_coordinates.y *= 2;
	mouse_coordinates.y -= 1;
	mouse_coordinates.z = 1;
	mouse_coordinates.w = 1;

	mat4 model = CreateTranslationMatrix(game_state->cam.world_p);

	mat4 inv = model*view*projection;
	glm::mat4x4 a;
	glm::vec4 b;

	memcpy(glm::value_ptr(a), &inv, sizeof(r32) * 16);
	memcpy(glm::value_ptr(b), &mouse_coordinates, sizeof(r32) * 4);

	a = glm::inverse(a);

	b = a * b;

	raycast.x = b.x;
	raycast.y = b.y;
	raycast.z = b.z;

}

void DrawVertices(RenderGroup *group){
    glBindVertexArray(group->vao);
    glBindBuffer(GL_ARRAY_BUFFER, group->vbo);
    glBufferData(GL_ARRAY_BUFFER, group->vertex_data_bytes, group->vertex_data.data, GL_STATIC_DRAW);

	u32 offset = 0;
	for(int i = 0 ; i < group->format.attrib_count; i++){
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, group->format.attribute_sizes[i], GL_FLOAT, GL_FALSE, group->format.vertex_size_bytes, (void*)(sizeof(r32) * offset));
		offset += group->format.attribute_sizes[i];
	}

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    mat4 projection = CreatePerspectiveMatrix(ToRadians(90.0f), 0.01f, 1000.0f, 1280.0f, 720.0f);
    mat4 view = LookAt(game_state->cam.world_p, game_state->cam.camera_dir + game_state->cam.world_p);

	glUseProgram(group->shader_program);
	ShaderSetUniform(group->shader_program, "u_view", view);
    ShaderSetUniform(group->shader_program, "u_projection", projection);

	GLint atlas = LoadTexture("c:/code/FPS/src/assets/atlas.bmp");

	int samplers[2] = { 0, 1 };
	ShaderSetUniform(group->shader_program, "u_textures[0]", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, atlas);

	glDrawArrays(group->primitive_mode, 0, group->vertex_count);


	for(int i = 0 ; i < group->format.attrib_count; i ++){
		glDisableVertexAttribArray(i);
	}
    glBindVertexArray(0);

	Raycast(view, projection);
}

void PushVertexData(r32* data, u32 data_size_bytes, RenderGroup* group){
	group->vertex_data.Add(data, data_size_bytes);
	group->vertex_data_bytes += data_size_bytes;
}

void PushVertexData(r32 data, u32 data_size_bytes, RenderGroup* group){
	group->vertex_data.Add(data);
	group->vertex_data_bytes += data_size_bytes;
}

void PushMesh(Entity e, RenderGroup *group){
	for (int i = 0; i < e.mesh->vertex_count; i++) {
		for(int a = 0; a < group->format.attrib_count; a++){
			switch (group->format.attributes[a]){
				case Attribute_vertexPosition : {
					PushVertexData((r32*)&(e.mesh->vertices[i].position), sizeof(v3), group);
				} break;
				case Attribute_Normals: {
					PushVertexData((r32*)&(e.mesh->vertices[i].normal), sizeof(v3), group);
				} break;
				case Attribute_TextureCoordinates: {
					PushVertexData((r32*)&(e.mesh->vertices[i].texture_coords), sizeof(v2), group);
				} break;
				case Attribute_worldPosition: {
					PushVertexData((r32*)&(e.world_p), sizeof(v3), group);
				} break;
				case Attribute_TextureIndex: {
					PushVertexData((r32)e.mesh->texture.id, sizeof(r32), group);
				} break;
			}
		}
		group->vertex_count += 1;
	}
}

global BVHTree* bvh_tree;

u32 max_parents = 0;

void Traverse(BVHTree* tree, u32 parents){
	if(tree->left){
		Traverse(tree->left, parents+1);
	}
	if(tree->right){
		Traverse(tree->right, parents+1);
	}
	if(!tree->left && !tree->right){
//		DebugOutput("%d\n", parents);
		if(parents > max_parents){
			max_parents = parents;
		}
	}

}
global u32 world_width = 2;
global u32 world_height = 2;
global u32 world_depth = 1;

void GenerateWorld(Block* world){
//	TIMED_FUNCTION

	block_map[BlockType_Furnace][Face_Top] = v2 { 6, 8 };
	block_map[BlockType_Furnace][Face_Bottom] = v2 { 6, 8 };
	block_map[BlockType_Furnace][Face_Left] = v2 { 6, 8 };
	block_map[BlockType_Furnace][Face_Right] = v2 { 6, 8 };
	block_map[BlockType_Furnace][Face_Front] = v2 { 3, 8 };
	block_map[BlockType_Furnace][Face_Back] = v2 { 5, 8 };

	block_map[BlockType_Dirt][Face_Top] = v2 { 8, 6 };
	block_map[BlockType_Dirt][Face_Bottom] = v2 { 8, 6 };
	block_map[BlockType_Dirt][Face_Left] = v2 { 8, 6 };
	block_map[BlockType_Dirt][Face_Right] = v2 { 8, 6 };
	block_map[BlockType_Dirt][Face_Front] = v2 { 8, 6 };
	block_map[BlockType_Dirt][Face_Back] = v2 { 8, 6 };

//	TIMED_BLOCK("make")
	for (u32 y = 0; y < world_height; y++) {
		for (u32 z = 0; z < world_depth; z++) {
			for (u32 x = 0; x < world_width; x++) {
				u32 index = world_width*world_depth*y + z*world_width + x;
				world[index].block_type = BlockType_Dirt;
				MakeBlock(&world[index]);
				world[index].world_p = { (r32)x, (r32)y, (r32)z };
			}
		}
	}
//	BLOCK_END

//	TIMED_BLOCK("rand")
	srand(__rdtsc());
	for(int i = 0 ; i < (world_width * world_height * world_depth); i ++){
		int j = rand() % (world_width * world_height * world_depth);
		Block tmp = world[i];
		if(!world[i].mesh ){
			int p = 4;
		}
		if(!world[j].mesh ){
			int p = 4;
		}
		world[i] = world[j];
		world[j] = tmp;
	}
//	BLOCK_END


	v3 min = { 0 };
	v3 max = { 0 };

	BVHTree* nodes = (BVHTree*)malloc(sizeof(BVHTree) * world_width * world_height * world_depth);

//	TIMED_BLOCK("tree")
	for (u32 i = 0; i < world_width*world_height*world_depth; i++) {
		min = world[i].world_p;
		min.x -= 0.5f;
		min.y -= 0.5f;
		min.z -= 0.5f;
		max = world[i].world_p;
		max.x += 0.5f;
		max.y += 0.5f;
		max.z += 0.5f;

		if(i == 0){
			bvh_tree = bvh_tree->Init(min , max, 1, &world[i]);
		}
		else{
			bvh_tree->Insert(min, max, &nodes[i], &world[i]);
		}
	}
//	BLOCK_END
	Traverse(bvh_tree, 0);
	DebugOutput("\n\nTree Depth: %d\n\n", max_parents);
}

RenderGroup* CreateRenderGroup(u32* attribs, u32 attrib_count, char* vertex_shader, char* fragment_shader, GLuint primitive_mode){
	RenderGroup *group = (RenderGroup*)Malloc(sizeof(RenderGroup));
	group->vertex_data = Array(1024);
	group->format.attrib_count = attrib_count;
	group->format.attribute_sizes = (u32*)Malloc(sizeof(u32) * attrib_count);
	group->format.attributes = (u32*)Malloc(attrib_count * sizeof(u32));
	group->shader_program = 0;
	group->primitive_mode = primitive_mode;
	memcpy(group->format.attributes, attribs, attrib_count*sizeof(u32));
	for(int i = 0 ; i < attrib_count; i ++){
		switch(attribs[i]){
			case Attribute_vertexPosition : 
			case Attribute_Normals: 
			case Attribute_worldPosition: {
				group->format.vertex_size_bytes += sizeof(v3);
				group->format.attribute_sizes[i] = 3;
			} break;

			case Attribute_TextureCoordinates:{
				group->format.vertex_size_bytes += sizeof(v2);
				group->format.attribute_sizes[i] = 2;
			} break;

			case Attribute_TextureIndex: {
				group->format.vertex_size_bytes += sizeof(u32);
				group->format.attribute_sizes[i] = 1;
			} break;
		}
	}
	ShaderCreate(vertex_shader, fragment_shader, &(group->shader_program));
	glGenVertexArrays(1, &(group->vao));
	glGenBuffers(1, &(group->vbo));
	return group;
}

void PushRect(RenderGroup* group, Box b, Dimension d) {
	Vertex v[6];
	v[0] = { 0 };
	v[1] = { 0 };
	v[2] = { 0 };
	v[3] = { 0 };
	v[4] = { 0 };
	v[5] = { 0 };

	if (d == Dimension_z){
		v[0].position.x = b.min.x;
		v[0].position.y = b.min.y;
		v[0].position.z = b.min.z;

		v[1].position.x = b.min.x;
		v[1].position.y = b.max.y;
		v[1].position.z = b.min.z;

		v[2].position.x = b.max.x;
		v[2].position.y = b.max.y;
		v[2].position.z = b.min.z;

		v[3] = v[2];

		v[4].position.x = b.max.x;
		v[4].position.y = b.min.y;
		v[4].position.z = b.min.z;

		v[5] = v[0];
	}

	if (d == Dimension_x){
		v[0].position.x = b.max.x;
		v[0].position.y = b.min.y;
		v[0].position.z = b.min.z;

		v[1].position.x = b.max.x;
		v[1].position.y = b.max.y;
		v[1].position.z = b.min.z;

		v[2].position.x = b.max.x;
		v[2].position.y = b.max.y;
		v[2].position.z = b.max.z;

		v[3] = v[2];

		v[4].position.x = b.max.x;
		v[4].position.y = b.min.y;
		v[4].position.z = b.max.z;

		v[5] = v[0];
	}

	if (d == Dimension_y){
		v[0].position.x = b.min.x;
		v[0].position.y = b.max.y;
		v[0].position.z = b.min.z;

		v[1].position.x = b.min.x;
		v[1].position.y = b.max.y;
		v[1].position.z = b.max.z;

		v[2].position.x = b.max.x;
		v[2].position.y = b.max.y;
		v[2].position.z = b.max.z;

		v[3] = v[2];

		v[4].position.x = b.max.x;
		v[4].position.y = b.max.y;
		v[4].position.z = b.min.z;

		v[5] = v[0];
	}

	for(int v_i = 0; v_i < 6; v_i++){
		PushVertexData((r32*)&(v[v_i].position), sizeof(v3), group);
		group->vertex_count++;
	}
}

void PushBox(RenderGroup* group, Box b){
	v3 min , max;
	min = b.min;
	max = b.max;
	max.z = min.z;
	PushRect(group, Box{min, max}, Dimension_z);

	min = b.min;
	max = b.max;
	max.y = min.y;
	PushRect(group, Box{min, max}, Dimension_y);

	min = b.min;
	max = b.max;
	max.x = min.x;
	PushRect(group, Box{min, max}, Dimension_x);

	min = b.min;
	max = b.max;
	min.z = max.z;
	PushRect(group, Box{min, max}, Dimension_z);

	min = b.min;
	max = b.max;
	min.y = max.y;
	PushRect(group, Box{min, max}, Dimension_y);

	min = b.min;
	max = b.max;
	min.x = max.x;
	PushRect(group, Box{min, max}, Dimension_x);
}

void PushLine(RenderGroup* group ,v3 start, v3 end, r32 thickness){
	r32 half_thickness = thickness / 2.0f;
	v3 diff = start-end;

	Box b = {0};

	b.min.x = start.x;
	b.max.x = end.x;
	b.min.y = start.y;
	b.max.y = end.y;
	b.min.z = start.z;
	b.max.z = end.z;

	if(diff.x){
		b.min.y -= half_thickness;
		b.max.y += half_thickness;
		b.min.z -= half_thickness;
		b.max.z += half_thickness;
	}
	if(diff.y){
		b.min.x -= half_thickness;
		b.max.x += half_thickness;
		b.min.z -= half_thickness;
		b.max.z += half_thickness;
	}
	if(diff.z){
		b.min.y -= half_thickness;
		b.max.y += half_thickness;
		b.min.x -= half_thickness;
		b.max.x += half_thickness;
	}
	PushBox(group, b);
}

void PushBoxOutline(RenderGroup* group ,Box b, r32 thickness){
	v3 start[12];
	v3 end[12];
	for(int i = 0; i < 12; i ++){
		start[i] = b.min;
		end[i] = b.max;
	}

	end[0].x = b.min.x;
	end[0].y = b.min.y;
	end[1].x = b.min.x;
	end[1].z = b.min.z;
	end[2].y = b.min.y;
	end[2].z = b.min.z;

	start[3].x = b.max.x;
	start[3].y = b.max.y;
	start[4].x = b.max.x;
	start[4].z = b.max.z;
	start[5].y = b.max.y;
	start[5].z = b.max.z;

	start[6].y = b.max.y;
	start[7].y = b.max.y;
	end[6].x = b.min.x;
	end[7].z = b.min.z;

	start[8].x = b.max.x;
	start[9].x = b.max.x;
	end[8].y = b.min.y;
	end[9].z = b.min.z;

	start[10].z = b.max.z;
	start[11].z = b.max.z;
	end[10].x = b.min.x;
	end[11].y = b.min.y;

	for(int i = 0; i < 12; i++){
		PushLine(group, start[i], end[i], thickness);
	}


}


Entity* CheckCollision(Entity* a, BVHTree* tree){
	v3 a_low, a_high;
	Entity* e = NULL;

	a_low.x = a->world_p.x;
	a_low.y = a->world_p.y;
	a_low.z = a->world_p.z;

	a_high.x = a->world_p.x;
	a_high.y = a->world_p.y;
	a_high.z = a->world_p.z;

	b32 collision = 1;
	if(a_low.x > tree->max.x){
		collision = 0;
	}
	if(a_low.y > tree->max.y){
		collision =  0;
	}
	if(a_low.z > tree->max.z){
		collision =  0;
	}
	if(a_high.x < tree->min.x){
		collision =  0;
	}
	if(a_high.y < tree->min.y){
		collision =  0;
	}
	if(a_high.z < tree->min.z){
		collision =  0;
	}
	if(collision){
		e = (Entity*)tree->e;
		if(tree->left){
			Entity* e = CheckCollision(a, tree->left);
			if(e){
				return e;
			}
		}
		if(tree->right){
			Entity* e = CheckCollision(a, tree->right);
			if(e){
				return e;
			}
		}
	}
	return e;
}

Entity* CheckCollision(v3 pos, BVHTree* tree) {
	Entity a;
	a.world_p = pos;
	return CheckCollision(&a, tree);
}

void CameraMove(){

	v3 unnormalised_movement = { 0 };

	r32 movement_speed = 10;
	if (game_input.move_forward.is_down) {
		unnormalised_movement += game_state->cam.camera_dir;
	}
	if (game_input.move_back.is_down) {
		unnormalised_movement -= game_state->cam.camera_dir;
	}
	if (game_input.move_right.is_down) {
		unnormalised_movement += Normalize(Cross(v3_up, game_state->cam.camera_dir));
	}
	if (game_input.move_left.is_down) {
		unnormalised_movement -= Normalize(Cross(v3_up, game_state->cam.camera_dir));
	}
	if (game_input.move_down.is_down) {
		unnormalised_movement.y = -1;
	}
	if (game_input.move_up.is_down) {
		unnormalised_movement.y = 1;
	}
	
	r32 sensitivity = 0.4f;
	if (game_input.mouse_diff.x > 100 || game_input.mouse_diff.y > 100){}
	else {
		if (game_input.mouse_diff.x != 0) {
			game_state->cam.camera_yaw += (game_input.mouse_diff.x * sensitivity);
		}
	
		if (game_input.mouse_diff.y != 0) {
			game_state->cam.camera_pitch += (game_input.mouse_diff.y * sensitivity);
			if (game_state->cam.camera_pitch > 90) {
				game_state->cam.camera_pitch = 90;
			}
			if (game_state->cam.camera_pitch < -90) {
				game_state->cam.camera_pitch = -90;
			}
		}
	
		r32 cos_yaw = cos(ToRadians(game_state->cam.camera_yaw));
		r32 sin_yaw = sin(ToRadians(game_state->cam.camera_yaw));
		r32 cos_pitch = cos(ToRadians(game_state->cam.camera_pitch));
		r32 sin_pitch = sin(ToRadians(game_state->cam.camera_pitch));
		game_state->cam.camera_dir.x = cos_yaw * cos_pitch;
		game_state->cam.camera_dir.z = sin_yaw * cos_pitch;
		game_state->cam.camera_dir.y = sin_pitch;
		game_state->cam.camera_dir = Normalize(game_state->cam.camera_dir);
	}
	
	v3 normalised_movement = Normalize(unnormalised_movement) * movement_speed * frame_delta;

	b32 valid_movement = 1;
	game_state->cam.world_p += normalised_movement;

	BVHTree* current = bvh_tree;

//	TIMED_BLOCK("Check Collision")
	if (CheckCollision(&game_state->cam, current)){
		valid_movement = 0;
	}
//	BLOCK_END
	if(!valid_movement){
		game_state->cam.world_p -= normalised_movement;
	}
}

void RenderGame(HWND window, IO* io_in, Memory memory, r32 in_frame_delta, Game_Input in_game_input, RECT client_rect) {
//	TIMED_FUNCTION
	persist b32 init = 0;
	persist RenderGroup *group_world;
	persist RenderGroup *group_debug;
	game_input = in_game_input;
	frame_delta = in_frame_delta;

	if (!init) {
		srand(0);
		init = 1;
		game_state = (Game_State*)memory.memory;
		SetMemoryArena( { &(game_state->d_memory), memory.size - (u32)sizeof(Game_State) });
		game_state->cam.camera_dir = { 0, 0, 1 };
		game_state->cam.world_p = { 5, 20, 5 };
		io = io_in;
		InitOpenGL(window);

		GenerateWorld(world);

		u32 world_attribs[] = { 
			Attribute_vertexPosition, 
			Attribute_Normals, 
			Attribute_TextureCoordinates, 
			Attribute_worldPosition, 
			Attribute_TextureIndex
		};

		group_world = CreateRenderGroup(world_attribs, ARRAYCOUNT(world_attribs), "c:/code/fps/src/shaders/cube_vertex.hlsl", "c:/code/fps/src/shaders/cube_fragment.hlsl", GL_TRIANGLES);

		u32 debug_attribs[] = { 
			Attribute_vertexPosition,
		};
		group_debug = CreateRenderGroup(debug_attribs, ARRAYCOUNT(debug_attribs), "c:/code/fps/src/shaders/debug_vertex.hlsl", "c:/code/fps/src/shaders/debug_fragment.hlsl", GL_TRIANGLES);
	}

	group_world->vertex_data_bytes = 0;
	group_world->vertex_data.count = 0;
	group_world->vertex_count = 0;

	group_debug->vertex_data_bytes = 0;
	group_debug->vertex_data.count = 0;
	group_debug->vertex_count = 0;

	v2 new_dimensions;
	new_dimensions.x = (r32)client_rect.right - client_rect.left;
	new_dimensions.y = (r32)client_rect.bottom - client_rect.top;
	if (new_dimensions != game_state->client_dimensions) {
		glViewport(0, 0, (GLuint)new_dimensions.x, (GLuint)new_dimensions.y);
		game_state->client_dimensions = new_dimensions;
	}

	CameraMove();

	
	wglSwapInterval(1);

	glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.4f, 0.6f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//	TIMED_BLOCK("Push Vert")
	for(int i = 0; i < world_width * world_height * world_depth; i ++){
		PushMesh(world[i], group_world);
	}
//	PushBoxOutline(group_debug, Box { v3 { -.5, -.5, -.5 }, v3 { .5, .5, .5 } }, 0.05f);
//	PushLine(group_debug ,game_state->cam.world_p, game_state->cam.world_p + raycast, 0.05f);
//	BLOCK_END

	Entity * e;
	Box b;
	if(e = CheckCollision(game_state->cam.world_p + raycast, bvh_tree)){
		b.min.x = e->world_p.x - 0.5f;
		b.min.y = e->world_p.y - 0.5f;
		b.min.z = e->world_p.z - 0.5f;
		b.max.x = e->world_p.x + 0.5f;
		b.max.y = e->world_p.y + 0.5f;
		b.max.z = e->world_p.z + 0.5f;
		PushBoxOutline(group_debug, b, 0.05f);
	}

//	TIMED_BLOCK("Draw Vert")
	DrawVertices(group_debug);
	DrawVertices(group_world);
//	BLOCK_END

	HDC device_context = GetDC(window);
	SwapBuffers(device_context);
	ReleaseDC(window, device_context);

//	if(most_recent_collision){
//		DebugOutput("%f\n", most_recent_collision->world_p.x);
//	}

}

