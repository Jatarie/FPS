#pragma once


#include "DataStructures.h"

#if 1
    #define ASSERT(expression) if(!expression){*(int*)0 = 0;}
#else
    #define ASSERT(expression)
#endif


#define b32 uint32_t

#include "maths.h"

struct File{
    u64 size;
    void* memory;
};

typedef File type_ReadEntireFile(char* file);
typedef void type_FreeEntireFile(File game_file);

struct IO{
    type_ReadEntireFile* ReadEntireFile;
    type_FreeEntireFile* FreeEntireFile;
};

static IO* io;

struct ColorRGBA{
    r32 r;
    r32 g;
    r32 b;
    r32 a;
};

inline ColorRGBA operator*(ColorRGBA a, r32 b){
    a.r *= b;
    a.g *= b;
    a.b *= b;
    ASSERT((a.r <= 1.0f) == 1);
    ASSERT((a.g <= 1.0f) == 1);
    ASSERT((a.b <= 1.0f) == 1);
    return a;
};

inline ColorRGBA operator*=(ColorRGBA a, r32 b){
    a.r *= b;
    a.g *= b;
    a.b *= b;
    ASSERT((a.r <= 1.0f) == 1);
    ASSERT((a.g <= 1.0f) == 1);
    ASSERT((a.b <= 1.0f) == 1);
    return a;
};

struct Rect{
	union {
		v2 bottom_left;
		struct {
			r32 left;
			r32 bottom;
		};
	};
	union {
		v2 top_right;
		struct {
			r32 right;
			r32 top;
		};
	};
};

enum Dimension{
	Dimension_x,
	Dimension_y,
	Dimension_z,
};

struct Game_Buffer{
    u32 width;
    u32 height;
    void* memory;
};

struct Input_Button_State{
    u32 transition_count;
    u32 is_down;
};

struct Input_Trigger_State{
    u8 value;
};

struct Input_Stick_State{
    s16 x;
    s16 y;
};

struct Mouse_State{
    s32 x;
    s32 y;
};

struct Game_Input{
    union{
        Input_Button_State buttons[14];
        struct{
            Input_Button_State move_forward;
            Input_Button_State move_back;
            Input_Button_State move_left;
            Input_Button_State move_right;
            Input_Button_State move_up;
            Input_Button_State move_down;
            Input_Button_State start;
            Input_Button_State back;
            Input_Button_State left_thumb;
            Input_Button_State right_thumb;
            Input_Button_State left_shoulder;
            Input_Button_State right_shoulder;
            Input_Button_State a;
            Input_Button_State b;
            Input_Button_State x;
            Input_Button_State y;
        };
    };
    Input_Trigger_State left_trigger;
    Input_Trigger_State right_trigger;
    Input_Stick_State left_stick;
    Input_Stick_State right_stick;
    Mouse_State mouse;
    Mouse_State mouse_diff;
};


#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
PACK(
struct Bitmap_Header{
    u16 header_field;
    u32 bmp_file_size_in_bytes;
    u16 RESERVED_1;
    u16 RESERVED_2;
    u32 pixel_data_offset;

    u32 set_to_40;
    s32 bmp_width;
    s32 bmp_height;
    u16 set_to_1;
    u16 bits_per_pixel;
    u32 compression_method;
    u32 raw_image_size;
    u32 horizontal_resolution_in_pels;
    u32 vertical_resolution_in_pels;
    u32 set_to_00;
    u32 set_to_000;
    u32 red_mask;
    u32 blue_mask;
    u32 green_mask;
}
);

struct Bitmap{
    Bitmap_Header* header;
    u32* pixels;
};

struct Memory{
    void* memory;
    u32 size;
};

global Game_Input game_input;
global r32 frame_delta;

enum AttributeTypes{
	Attribute_VertexPosition,
	Attribute_WorldPosition,
	Attribute_Normals,
	Attribute_TextureCoordinates,
	Attribute_TextureIndex,
	Attribute_Color,
};

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
    u32 diffuse;
    u32 specular;
};

struct Mesh {
	Material material;
	Texture texture;

	u32 vertex_count;
	Vertex* vertices;

	u32 index_count;
	u32* indices;
};

struct Model{
    u32 mesh_count;
    Mesh** meshes;
};

struct Raycast{
	v3 origin;
	v3 direction;
};

enum SpriteType{
	Sprite_DirtTop,
	Sprite_DirtSide,

	Sprite_FurnaceFront,
	Sprite_FurnaceBack,
	Sprite_FurnaceTop,
	Sprite_FurnaceBottom,
	Sprite_FurnaceLeft,
	Sprite_FurnaceRight,

	Sprite_COUNT,
};

enum Faces{
	Face_Top,
	Face_Bottom,
	Face_Front,
	Face_Back,
	Face_Left,
	Face_Right,
};

enum BlockType {
	BlockType_Furnace,
	BlockType_Dirt,

	BlockType_COUNT
};

struct Entity{
	Mesh* mesh;
	v3 world_p;
	Box bounding_box;
	r32 velocity;
	r32 acceleration;
};

struct Block : Entity{
	BlockType block_type;
};

struct Camera : Entity{
    v3 camera_dir;
    r32 camera_yaw;
    r32 camera_pitch;
};

v2 block_map[BlockType_COUNT][6];


static float cube_vertices[] = {
	// positions         // normals         // texture coords
	//BACK
	0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // TR
	0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // BR
	-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // TL
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // BL
	-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // TL
	0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // BR

	//FRONT
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

	//LEFT
	-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // TR
	-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, //TL
	-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // BR
	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, //BL
	-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // BR
	-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, //TL

	//RIGHT
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // TR
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // TL
	0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // BR
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // BL
	0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // BR
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // TL

	//DOWN
	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
	0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
	0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

	//UP
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
	u32* attributes;
	u32* attribute_sizes;
};

enum TextureMap{
	TextureMap_Atlas,
	TextureMap_UI,
	TextureMap_COUNT,
};

struct RenderGroup{
	GLuint vao, vbo, shader_program, primitive_mode;
	u32 vertex_data_bytes;
	u32 vertex_count;
	VertexDataFormat format;
	Array vertex_data;
	TextureMap texture_map;
};

enum RenderGroups{
	RenderGroups_World,
	RenderGroups_Debug,
	RenderGroups_UI,
	RenderGroups_Raycast,
	RenderGroups_COUNT,
};

#include "collision.h"

struct GameState{
	b32 initialised;
	HGLRC gl_render_context;
	Camera cam;
	v2 client_dimensions;
	RenderGroup* render_groups[RenderGroups_COUNT];
	Block world[10000];
	Hash_Table* loaded_textures;
	Array raycast_collisions;

	OctTree* collision_tree;

	Memory_Arena memory_arena;
	GLuint textures[TextureMap_COUNT];
};

global GameState* game_state;


