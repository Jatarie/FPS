#include "game.h"
#include "mygl.h"
#include "shader.h"
#include "maths.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


static u32 init = 0;

struct Cube{
    v3 p;
};

#include <stdio.h>
// #define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
// #include "stb_truetype.h"

// unsigned char ttf_buffer[1<<25];

// struct Font{
//     s32 ascent;
//     s32 descent;
//     s32 line_gap;
//     s32 baseline;
//     stbtt_fontinfo fontinfo;
//     r32 scale;
// };

// struct Text{
//     GLuint texture;
//     u8 character;
//     u8* bitmap;
//     u32 height;
//     u32 width;
//     u32 advance;
//     s32 x0, x1, y0, y1;
// };

// Font InitFont(char* font_file, float scale){
//     Font font = {0};
//     FILE* fp = fopen(font_file, "rb");
//     fread(ttf_buffer, 1, 1<<25, fp);
//     stbtt_InitFont(&(font.fontinfo), ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0));
//     font.scale = stbtt_ScaleForPixelHeight(&(font.fontinfo), scale);
//     stbtt_GetFontVMetrics(&(font.fontinfo), &(font.ascent), &(font.descent), &(font.line_gap));
//     font.ascent = (int)(font.scale * font.ascent);
//     font.descent = (int)(font.scale * font.descent);
//     fclose(fp);
//     font.baseline = font.ascent;
//     return font;
// };

// Text stbthing(char c, Font* font){
//     unsigned char *bitmap;
//     int w,h = 0;

//     bitmap = stbtt_GetCodepointBitmap(&(font->fontinfo), font->scale, font->scale, c, &w, &h, 0, 0);

//     int x0, x1, y0, y1;

//     stbtt_GetCodepointBitmapBox(&(font->fontinfo), c, font->scale, font->scale, &x0, &y0, &x1, &y1);

//     int advance, left_side_bearing;
//     stbtt_GetCodepointHMetrics(&(font->fontinfo), c, &advance, &left_side_bearing);

//     u32* bitmap_32bit = (u32*)Malloc(sizeof(u32) * h * w);
//     for(int y = 0; y < h; y ++){
//         for(int x = 0; x < w; x ++){
//             u8 alpha = bitmap[x + y * w];
//             u8* byte = (u8*)(&bitmap_32bit[x + y * w]);
//             *byte++ = 0x00;
//             *byte++ = 0x00;
//             *byte++ = 0x00;
//             *byte = alpha;
//         }
//     }

//     Text text = {0};
//     text.character = c;
//     text.bitmap = (u8*)bitmap_32bit;
//     text.height = h;
//     text.width = w;
//     text.advance = advance;
//     text.x0 = x0;
//     text.x1 = x1;
//     text.y0 = y0;
//     text.y1 = y1;


//     unsigned int texture;
//     glGenTextures(1, &texture);
//     glBindTexture(GL_TEXTURE_2D, texture);
//     glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, text.width, text.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, text.bitmap);

//     // set texture options
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//     text.texture = texture;
//     return text;
// }

//ASSUMES NULL TERMINATION
// void DrawText(char* characters,  r32 scale, v2 position, Text* character_data){
//     float letter_vertices[] = {
//         // positions // colors // texture coords
//         -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
//         0.5f, 0.5f, 0.0f,   1.0f, 1.0f, // top right
//         -0.5f, 0.5f, 0.0f,  0.0f, 1.0f, // top left

//         -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
//         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // bottom right
//         0.5f, 0.5f, 0.0f,   1.0f, 1.0f, // top right

//     };
//     u32 i = 0;
//     GLuint vbo;
//     GLuint letter_shader_program;
//     ShaderCreate("c:/code/fps/src/shaders/letter_vertex.hlsl", "c:/code/fps/src/shaders/letter_fragment.hlsl", &letter_shader_program);
//     glGenBuffers(1, &vbo);
//     glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(letter_vertices), letter_vertices, GL_STATIC_DRAW);

//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//     glEnableVertexAttribArray(1);

//     glBindTexture(GL_TEXTURE_2D, character_data[0].texture);

//     while(characters[i] != '\0'){
//         glUseProgram(letter_shader_program);
//         glDrawArrays(GL_TRIANGLES, 0, 6);
//         i++;
//     }
// }


#include "mygltest.cpp"
#include "mygltestmesh.cpp"
#include "mygltesttext.cpp"

void RenderGame(HWND window, IO* io_in, Memory memory, r32 frame_delta, Game_Input game_input, RECT client_rect){


    // static int font_init = false;
    // static Text characters[127];
    // static Font font = {0};

    if(!init){
        init = 1;
        game_state = (Game_State*)memory.memory;
        SetMemoryArena({&(game_state->d_memory), memory.size - (u32)sizeof(Game_State)});
        game_state->camera_dir = {0, 0, 1};
        io = io_in;
        InitOpenGL(window);

        // if (!font_init){
        //     font_init = 1;`
        //     font = InitFont("C:/Windows/Fonts/arial.ttf", 40.0f);
        //     for(int i = 32; i < 127; i ++){
        //         characters[i] = stbthing(i, &font);
        //     }
        // }

        //This is very important apparently
        // glPixelStorei(GL_UNPACK_ALIGNMENT,1);

        // Text character = characters['P'];

        // glBindTexture(GL_TEXTURE_2D, character.texture);


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
            game_state->yaw += (game_input.mouse_diff.x * sensitivity);

        }

        if(game_input.mouse_diff.y != 0){
            game_state->pitch += (game_input.mouse_diff.y * sensitivity);
            if(game_state->pitch > 90){
                game_state->pitch = 90;
            }
            if( game_state->pitch < -90){
                game_state->pitch = -90;
            }
        }

        r32 cos_yaw = cos(ToRadians(game_state->yaw));
        r32 sin_yaw = sin(ToRadians(game_state->yaw));
        r32 cos_pitch = cos(ToRadians(game_state->pitch));
        r32 sin_pitch = sin(ToRadians(game_state->pitch));
        game_state->camera_dir.x = cos_yaw * cos_pitch;
        game_state->camera_dir.z = sin_yaw * cos_pitch;
        game_state->camera_dir.y = sin_pitch;
        game_state->camera_dir = Normalize(game_state->camera_dir);
    }

    normalised_movement = Normalize(normalised_movement) * movement_speed * frame_delta;
    game_state->camera_p += normalised_movement;


    wglSwapInterval(1);
    TestAssimp(window);
    return;
}

