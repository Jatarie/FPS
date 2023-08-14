#include "game.h"
#include "mygl.h"
#include "shader.h"
#include "maths.h"
#include "DataStructures.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include "stb_truetype.h"


unsigned char ttf_buffer[1<<25];

struct Font{
    s32 ascent;
    s32 descent;
    s32 line_gap;
    s32 baseline;
    stbtt_fontinfo fontinfo;
    r32 scale;
};

struct Text{
    GLuint texture;
    u8 character;
    u8* bitmap;
    u32 height;
    u32 width;
    r32 advance;
    r32 left_bearing;
    s32 x0, x1, y0, y1;

    GLuint vao, ebo, vbo, shader;
};

Font InitFont(char* font_file, float scale){
    Font font = {0};
    FILE* fp = fopen(font_file, "rb");
    fread(ttf_buffer, 1, 1<<25, fp);
    stbtt_InitFont(&(font.fontinfo), ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0));
    font.scale = stbtt_ScaleForPixelHeight(&(font.fontinfo), scale);
    stbtt_GetFontVMetrics(&(font.fontinfo), &(font.ascent), &(font.descent), &(font.line_gap));
    font.ascent = (int)(font.scale * font.ascent);
    font.descent = (int)(font.scale * font.descent);
    fclose(fp);
    font.baseline = font.ascent;
    return font;
};

Text stbthing(char c, Font* font){
    unsigned char *bitmap;
    int w,h = 0;

    bitmap = stbtt_GetCodepointBitmap(&(font->fontinfo), font->scale, font->scale, c, &w, &h, 0, 0);

    int x0, x1, y0, y1;

    stbtt_GetCodepointBitmapBox(&(font->fontinfo), c, font->scale, font->scale, &x0, &y0, &x1, &y1);

    int advance, left_bearing;
    stbtt_GetCodepointHMetrics(&(font->fontinfo), c, &advance, &left_bearing);

    Text text = {0};
    text.character = c;
    text.bitmap = bitmap;
    text.height = h;
    text.width = w;
    text.advance = advance * font->scale;
    text.left_bearing = left_bearing * font->scale;
    text.x0 = x0;
    text.x1 = x1;
    text.y0 = y0;
    text.y1 = y1;

    // r32 vertices[] = {
    //     0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
    //     0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    //     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
    //     -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
    // };

    r32 height_scaler = text.height / (game_state->client_dimensions.y / 2);
    r32 width_scaler = text.width / (game_state->client_dimensions.x / 2);

    float top = 0;
    float left = 0;
    float bottom = height_scaler;
    float right = width_scaler;
    r32 vertices[] = {
        right, bottom, 0.0f, 1.0f, 1.0f, //TR
        right, top, 0.0f, 1.0f, 0.0f, //BR
        left, top, 0.0f, 0.0f, 0.0f, //BL
        left, bottom, 0.0f, 0.0f, 1.0f // TL
    };

    u32 indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &text.vao);
    glBindVertexArray(text.vao);

    glGenBuffers(1, &text.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, text.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenBuffers(1, &text.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, text.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    if(text.bitmap){
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, text.width, text.height, 0, GL_RED, GL_UNSIGNED_BYTE, text.bitmap);

        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        text.texture = texture;
        glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    }
    return text;
}


void DrawCharacter(Text c, GLuint shader, v2 position){

    glBindTexture(GL_TEXTURE_2D, c.texture);

    mat4 texture_matrix = { 1, 0, 0, 0,
                            0, -1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1 };

    r32 ratio = (r32)c.width / (r32)c.height;

    r32 height_scaler = c.height / (game_state->client_dimensions.y / 2);
    r32 width_scaler = c.width / (game_state->client_dimensions.x / 2);

    r32 x_gl = ((position.x / game_state->client_dimensions.x) * 2) - 1;
    r32 y_gl = ((position.y / game_state->client_dimensions.y) * 2) - 1;


    // mat4 scale = CreateScaleMatrix(v3{1.0f * width_scaler, 1.0f * height_scaler, 1.0f} * 0.1);
    mat4 scale = CreateScaleMatrix(v3{1.0f, 1.0f, 1.0f});
    mat4 rotation = CreateRotationMatrix(Normalize({1.0f, 1.0f, 1.0f}), 0);
    mat4 translation = CreateTranslationMatrix({x_gl, y_gl, 0});
    mat4 model = scale * rotation * translation * texture_matrix;

    mat4 projection = CreatePerspectiveMatrix(ToRadians(120.0f), 0.01f, 1000.0f, 1280.0f, 720.0f);

    mat4 view = LookAt(game_state->camera_p, game_state->camera_dir + game_state->camera_p);

    ShaderSetUniform(shader, "model", model);
    ShaderSetUniform(shader, "view", view);
    ShaderSetUniform(shader, "projection", projection);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glUseProgram(shader);
    glBindVertexArray(c.vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void DrawCharacters(char* string, v2 position, Text* characters, GLuint shader){
    for(char i = 0; i < strlen(string); i ++){
        char cur_char = string[i];
        Text c = characters[cur_char];
        position.x += c.left_bearing;
        DrawCharacter(c, shader, v2{position.x + c.x0, position.y + c.y0});
        position.x += c.advance;
    }
}

void TestDrawText(HWND window){

    static int font_init = false;
    static Text characters[127];
    static Font font = {0};
    static u32 init = 0;
    static GLuint shader;

    glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    if(!init){
        init = 1;
        ShaderCreate("C:\\Code\\FPS\\src\\shaders\\letter_vertex.hlsl", "C:\\Code\\FPS\\src\\shaders\\letter_fragment.hlsl", &shader);

        if (!font_init){
            font_init = 1;
            font = InitFont("C:/Windows/Fonts/arial.ttf", 30.0f);
            for(int i = 32; i < 127; i ++){
                characters[i] = stbthing(i, &font);
            }
        }
    }
    DrawCharacters("what the fuck did you just say about me", v2{0, 50}, characters, shader);
    DrawCharacters("you little bitch", v2{0, 110}, characters, shader);

    HDC device_context = GetDC(window);
    SwapBuffers(device_context);
    ReleaseDC(window, device_context);
}