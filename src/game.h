#pragma once


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


#include "windows.h"
#include "gl/GL.h"

