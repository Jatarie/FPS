#include <stdio.h>
#include "glm.hpp"
#include "type_ptr.hpp"

#include "mygl.h"
#include "game.h"

#include "maths.h"
#include "datastructures.h"
#include "profile.h"

#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "collision.cpp"

GLuint LoadTexture(char* path_to_image){
	if(!game_state->loaded_textures){
		game_state->loaded_textures = game_state->loaded_textures->Create(100);
	}
	
	GLuint test_if_loaded = game_state->loaded_textures->Get(game_state->loaded_textures, path_to_image);
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
	
	game_state->loaded_textures->Add(game_state->loaded_textures, path_to_image, texture);
	return texture;
}

void LoadAllTextures(){
	GLint atlas = LoadTexture("c:/code/FPS/src/assets/atlas.bmp");
	game_state->textures[TextureMap_Atlas] = atlas;
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

	//TODO MS: MYSTERIOUS RANDOM NUMBER
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

void RaycastThing(mat4 view, mat4 projection){ 	
	TIMED_FUNCTION
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

	mat4 inv = MatrixInverse(model*view*projection);
	v4 result = Multiply(mouse_coordinates, inv);

	raycast.x = result.x;
	raycast.y = result.y;
	raycast.z = result.z;

}

void DrawVertices(RenderGroup *group){
	TIMED_FUNCTION
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

    mat4 projection = CreatePerspectiveMatrix(ToRadians(100.0f), 0.01f, 1000.0f, game_state->client_dimensions.x, game_state->client_dimensions.y);
    mat4 view = LookAt(game_state->cam.world_p, game_state->cam.camera_dir + game_state->cam.world_p);
	static u32 thingo = 0;

	glUseProgram(group->shader_program);
	ShaderSetUniform(group->shader_program, "u_view", view);
    ShaderSetUniform(group->shader_program, "u_projection", projection);

//	GLint atlas = LoadTexture("c:/code/FPS/src/assets/atlas.bmp");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, game_state->textures[group->texture_map]);

	glDrawArrays(group->primitive_mode, 0, group->vertex_count);


	for(int i = 0 ; i < group->format.attrib_count; i ++){
		glDisableVertexAttribArray(i);
	}
    glBindVertexArray(0);

	RaycastThing(view, projection);
}

void PushVertexData(r32* data, u32 data_size_bytes, RenderGroup* group){
	group->vertex_data.Add(data, data_size_bytes);
	group->vertex_data_bytes += data_size_bytes;
}

void PushVertexData(r32 data, u32 data_size_bytes, RenderGroup* group){
	group->vertex_data.Add(&data);
	group->vertex_data_bytes += data_size_bytes;
}

void PushMesh(Entity e, RenderGroup *group){
	TIMED_FUNCTION
	for (int i = 0; i < e.mesh->vertex_count; i++) {
		for(int a = 0; a < group->format.attrib_count; a++){
			switch (group->format.attributes[a]){
				case Attribute_VertexPosition : {
					PushVertexData((r32*)&(e.mesh->vertices[i].position), sizeof(v3), group);
				} break;
				case Attribute_Normals: {
					PushVertexData((r32*)&(e.mesh->vertices[i].normal), sizeof(v3), group);
				} break;
				case Attribute_TextureCoordinates: {
					PushVertexData((r32*)&(e.mesh->vertices[i].texture_coords), sizeof(v2), group);
				} break;
				case Attribute_WorldPosition: {
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

//global OctTree* octtree;

u32 max_parents = 0;


global u32 world_width = 64;
global u32 world_height = 1;
global u32 world_depth = 64;

float Perlin(v3 in){
	TIMED_FUNCTION
	in.x /= world_width;
	in.y /= world_height;
	in.z /= world_depth;
	return 3.0f;
}

void GenerateWorld(Block* world){
	TIMED_FUNCTION
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

	Box b;
	b.min = v3{-10, -10, -10};
	b.max = v3{(r32)world_width+10, (r32)world_height+10, (r32)world_depth+10};
	game_state->collision_tree = (OctTree*)Malloc(sizeof(OctTree));
	game_state->collision_tree->Init(0, b);

	for (u32 y = 0; y < world_height; y++) {
		for (u32 z = 0; z < world_depth; z++) {
			for (u32 x = 0; x < world_width; x++) {
				u32 index = world_width*world_depth*y + z*world_width + x;
				world[index].block_type = BlockType_Dirt;
				world[index].bounding_box.min = v3 { - .5, - .5, - .5 };
				world[index].bounding_box.max = v3 { + .5, + .5, + .5 };
				MakeBlock(&world[index]);
				r32 height = sin((2 * 3.1415 * x)/(world_width/2.0f));
				height += 2 * sin((2 * 3.1415 * z)/(world_depth/2.0f));

				world[index].world_p = { (r32)x+.5f, (r32)((s32)height), (r32)z+.5f };

				game_state->collision_tree->Insert(&world[index]);
			}
		}
	}
}

RenderGroup* CreateRenderGroup(u32* attribs, u32 attrib_count, char* vertex_shader, char* fragment_shader, GLuint primitive_mode, TextureMap texture_map){
	TIMED_FUNCTION
	RenderGroup *group = (RenderGroup*)Malloc(sizeof(RenderGroup));
	group->vertex_data = Array(1024, 4);
	group->format.attrib_count = attrib_count;
	group->format.attribute_sizes = (u32*)Malloc(sizeof(u32) * attrib_count);
	group->format.attributes = (u32*)Malloc(attrib_count * sizeof(u32));
	group->format.vertex_size_bytes = 0;
	group->shader_program = 0;
	group->primitive_mode = primitive_mode;
	group->texture_map = texture_map;
	memcpy(group->format.attributes, attribs, attrib_count*sizeof(u32));
	for(int i = 0 ; i < attrib_count; i ++){
		switch(attribs[i]){
			case Attribute_VertexPosition : 
			case Attribute_Normals: 
			case Attribute_WorldPosition:
			case Attribute_Color: {
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

void PushRect(RenderGroup* group, Box b, Dimension d, v3 color){
	TIMED_FUNCTION
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
		PushVertexData((r32*)&v[v_i].position, sizeof(v3), group);
		PushVertexData((r32*)&color, sizeof(v3), group);
		group->vertex_count++;
	}
}

void PushBox(RenderGroup* group, Box b, v3 color){
	TIMED_FUNCTION
	v3 min , max;
	min = b.min;
	max = b.max;
	max.z = min.z;
	PushRect(group, Box{min, max}, Dimension_z, color);

	min = b.min;
	max = b.max;
	max.y = min.y;
	PushRect(group, Box{min, max}, Dimension_y, color);

	min = b.min;
	max = b.max;
	max.x = min.x;
	PushRect(group, Box{min, max}, Dimension_x, color);

	min = b.min;
	max = b.max;
	min.z = max.z;
	PushRect(group, Box{min, max}, Dimension_z, color);

	min = b.min;
	max = b.max;
	min.y = max.y;
	PushRect(group, Box{min, max}, Dimension_y, color);

	min = b.min;
	max = b.max;
	min.x = max.x;
	PushRect(group, Box{min, max}, Dimension_x, color);
}

void PushLine(RenderGroup* group ,v3 start, v3 end, r32 thickness, v3 color){
	TIMED_FUNCTION
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
	PushBox(group, b, color);
}

void PushBoxOutline(RenderGroup* group ,Box b, r32 thickness, v3 color){
	TIMED_FUNCTION
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
		PushLine(group, start[i], end[i], thickness, color);
	}
}

void CameraMove(){
	TIMED_FUNCTION

	v3 unnormalised_movement = { 0 };

	r32 movement_speed = 10;
	if (game_input.move_forward.is_down) {
		unnormalised_movement += game_state->cam.camera_dir;
		unnormalised_movement.y = 0;
	}
	if (game_input.move_back.is_down) {
		unnormalised_movement -= game_state->cam.camera_dir;
		unnormalised_movement.y = 0;
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
			if (game_state->cam.camera_pitch > 89.9f) {
				game_state->cam.camera_pitch = 89.9f;
			}
			if (game_state->cam.camera_pitch < -89.9f) {
				game_state->cam.camera_pitch = -89.9f;
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

	if(game_input.move_up.is_down && game_input.move_up.transition_count == 1){
		game_state->cam.velocity = 5.0f;
	}

//	game_state->cam.velocity = -9.8 * frame_delta * 2 + game_state->cam.velocity;
//	r32 delta_y = game_state->cam.velocity * frame_delta + (0.5f * 9.8 * frame_delta*frame_delta);
//	normalised_movement.y = normalised_movement.y + delta_y;

	b32 valid_movement = true;

	OctTree* current = game_state->collision_tree;

	Entity* collision_entity;


	v3 test_position = game_state->cam.world_p;
	test_position.x += normalised_movement.x;
	if (collision_entity = CheckCollision(test_position, game_state->cam.bounding_box, current)){
		test_position.x -= normalised_movement.x;
	}
	test_position.y += normalised_movement.y;
	if (collision_entity = CheckCollision(test_position, game_state->cam.bounding_box, current)){
		test_position.y -= normalised_movement.y;
		game_state->cam.velocity = 0;
	}
	test_position.z += normalised_movement.z;
	if (collision_entity = CheckCollision(test_position, game_state->cam.bounding_box, current)){
		test_position.z -= normalised_movement.z;
	}

	game_state->cam.world_p = test_position;

	
}


v3 Colors[] = {
			   v3{1, 0 ,0},
			   v3{0, 1 ,0},
			   v3{0, 0 ,1},
			   v3{0, 1 ,1},
			   v3{1, 1 ,1},
			   v3{0.5, 0.5 ,1},
};

void DebugCollisionOutlines(RenderGroup* group ,OctTree* tree){
	PushBoxOutline(group, tree->bounding_box, 0.05f * tree->depth * 2 + 0.05f, Colors[tree->depth]);
	if(tree->children){
		for(int i = 0; i < 8; i ++){
			DebugCollisionOutlines(group, tree->children[i]);
		}
	}
}

void InitRenderGroups(){
		u32 world_attribs[] = { 
			Attribute_VertexPosition, 
			Attribute_Normals, 
			Attribute_TextureCoordinates, 
			Attribute_WorldPosition, 
			Attribute_TextureIndex
		};

		game_state->render_groups[RenderGroups_World] = CreateRenderGroup(world_attribs, ARRAYCOUNT(world_attribs), "c:/code/fps/src/shaders/cube_vertex.hlsl", "c:/code/fps/src/shaders/cube_fragment.hlsl", GL_TRIANGLES, TextureMap_Atlas);

		u32 debug_attribs[] = { 
			Attribute_VertexPosition,
			Attribute_Color,
		};
		game_state->render_groups[RenderGroups_Debug]  = CreateRenderGroup(debug_attribs, ARRAYCOUNT(debug_attribs), "c:/code/fps/src/shaders/debug_vertex.hlsl", "c:/code/fps/src/shaders/debug_fragment.hlsl", GL_TRIANGLES, TextureMap_Atlas);

		game_state->render_groups[RenderGroups_Raycast] = CreateRenderGroup(debug_attribs, ARRAYCOUNT(debug_attribs), "c:/code/fps/src/shaders/debug_vertex.hlsl", "c:/code/fps/src/shaders/debug_fragment.hlsl", GL_TRIANGLES, TextureMap_Atlas);
		game_state->render_groups[RenderGroups_UI] = CreateRenderGroup(debug_attribs, ARRAYCOUNT(debug_attribs), "c:/code/fps/src/shaders/ui_vertex.hlsl", "c:/code/fps/src/shaders/ui_fragment.hlsl", GL_TRIANGLES, TextureMap_Atlas);

}

void RenderGame(HWND window, IO* io_in, Memory_Arena* memory, r32 in_frame_delta, Game_Input in_game_input, RECT client_rect) {
	TIMED_FUNCTION
	persist b32 init = 0;
	game_input = in_game_input;
	frame_delta = in_frame_delta;

	if (!init) {
		srand(0);
		init = 1;
		SetMemoryArena(memory);

		io = io_in;
		if(memory->list[0].block_size == memory->list[0].max_block_size){
			game_state = (GameState*)Malloc(sizeof(GameState));
			GetRenderContext(window);
			SetUpGL();
			InitRenderGroups();
			game_state->raycast_collisions = Array(8, sizeof(Entity*));
			GenerateWorld(game_state->world);

			for(int i = 0; i < world_width * world_height * world_depth; i ++){
				PushMesh(game_state->world[i], game_state->render_groups[RenderGroups_World]);
			}

			game_state->cam.camera_dir = { 0, 0, 1 };
			game_state->cam.world_p = { 5, 20, 5 };
			game_state->cam.bounding_box.min = v3 { -.1f, -1.5f, -.1f };
			game_state->cam.bounding_box.max = v3 { +.1f, +.5f, +.1f };

			game_state->client_dimensions.x = client_rect.right;
			game_state->client_dimensions.y = client_rect.bottom;

			Box b;
			b.min = v3 { -1.0f/game_state->client_dimensions.x*10, -1.0f/game_state->client_dimensions.y, 0 };
			b.max = v3 { 1.0f/game_state->client_dimensions.x*10, 1.0f/game_state->client_dimensions.y, 0 };
			PushBox( game_state->render_groups[RenderGroups_UI], b, v3 { 1, 1, 1 });

			b.min = v3 { -1.0f/game_state->client_dimensions.x, -1.0f/game_state->client_dimensions.y*10, 0};
			b.max = v3 { 1.0f/game_state->client_dimensions.x, 1.0f/game_state->client_dimensions.y*10, 0 };
			PushBox(game_state->render_groups[RenderGroups_UI], b, v3 { 1, 1, 1 });

			LoadAllTextures();
		}
		else{
			game_state = (GameState*)memory->memory;
			SetUpGL();
		}

//		DebugCollisionOutlines(game_state->render_groups[RenderGroups_Debug], octtree);
	}


	game_state->render_groups[RenderGroups_Raycast]->vertex_data_bytes = 0;
	game_state->render_groups[RenderGroups_Raycast]->vertex_data.count = 0;
	game_state->render_groups[RenderGroups_Raycast]->vertex_count = 0;

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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	game_state->raycast_collisions.Clear();
	CheckCollisionRayWorld(Raycast { game_state->cam.world_p, raycast * 20 }, game_state->collision_tree);

	r32 distance = 1000000000.0f;
	Entity* collided_entity = NULL;
	for(u32 i = 0; i < game_state->raycast_collisions.count; i++){
		Entity* e = ((Entity**)(game_state->raycast_collisions.data))[i];
		v3 distance_to_entity = e->world_p - game_state->cam.world_p;
		r32 magnitude = abs(Inner(distance_to_entity, distance_to_entity));
		if(magnitude < distance){
			collided_entity = e;
			distance = magnitude;
		}
	}
	if(collided_entity){
		Box b;
		b.min.x = collided_entity->world_p.x - 0.5f;
		b.min.y = collided_entity->world_p.y - 0.5f;
		b.min.z = collided_entity->world_p.z - 0.5f;
		b.max.x = collided_entity->world_p.x + 0.5f;
		b.max.y = collided_entity->world_p.y + 0.5f;
		b.max.z = collided_entity->world_p.z + 0.5f;
		PushBoxOutline(game_state->render_groups[RenderGroups_Raycast], b, 0.05f, v3{1, 0, 1});
	}

	DrawVertices(game_state->render_groups[RenderGroups_Raycast]);
	DrawVertices(game_state->render_groups[RenderGroups_Debug]);
	DrawVertices(game_state->render_groups[RenderGroups_World]);
	DrawVertices(game_state->render_groups[RenderGroups_UI]);

	HDC device_context = GetDC(window);
	SwapBuffers(device_context);
	ReleaseDC(window, device_context);
	glEnd();

}
