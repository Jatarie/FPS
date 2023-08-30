#pragma once
#include "game.h"
#include "wglext.h"
#include <gl/gl.h>

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
    GLuint diffuse;
    GLuint specular;
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
	r32 velocity;
	r32 acceleration;
};

#include "DataStructures.h"

struct Block : Entity{
	BlockType block_type;
};

struct Camera : Entity{
    v3 camera_dir;
    r32 camera_yaw;
    r32 camera_pitch;
};

v2 block_map[BlockType_COUNT][6];



//global v2 sprite_coordinates[32*32];


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

struct RenderGroup{
	GLuint vao, vbo, shader_program, primitive_mode;
	Array vertex_data;
	u32 vertex_data_bytes;
	u32 vertex_count;
	VertexDataFormat format;
};

struct Game_State{
	HGLRC gl_render_context;
	Camera cam;
	v2 client_dimensions;

	void* d_memory;
};


static Game_State* game_state;

typedef signed long long int khronos_ssize_t;
typedef khronos_ssize_t GLsizeiptr;
typedef char GLchar;
#define GL_ARRAY_BUFFER 0x8892
#define GL_STATIC_DRAW 0x88E4
#define GL_VERTEX_SHADER 0x8B31
#define GL_COMPILE_STATUS 0x8B81
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_LINK_STATUS 0x8B82
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_MIRRORED_REPEAT 0x8370


typedef void WINAPI type_glGenVertexArrays( GLsizei n, GLuint *arrays);
typedef void WINAPI type_glBindVertexArray(GLuint array);
typedef void WINAPI type_glDrawArrays(GLenum mode, GLint first, GLsizei count);
typedef void WINAPI type_glGenBuffers(GLsizei n, GLuint* buffers);
typedef void WINAPI type_glBindBuffer(GLenum target, GLuint buffers);
typedef void WINAPI type_glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void WINAPI type_glVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
typedef void WINAPI type_glEnableVertexAttribArray(GLuint index);
typedef void WINAPI type_glDisableVertexAttribArray(GLuint index);
typedef void WINAPI type_glGenerateMipmap(GLenum target);
typedef GLuint WINAPI type_glCreateShader(GLenum type);
typedef void WINAPI type_glShaderSource(GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length);
typedef void WINAPI type_glCompileShader(GLuint shader);
typedef void WINAPI type_glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
typedef void WINAPI type_glGetShaderiv(GLuint shader, GLenum pname, GLint *params);
typedef GLuint WINAPI type_glCreateProgram();
typedef void WINAPI type_glAttachShader(GLuint program, GLuint shader);
typedef void WINAPI type_glLinkProgram(GLuint program);
typedef void WINAPI type_glGetProgramiv(GLuint program, GLenum pname, GLint *params);
typedef void WINAPI type_glUseProgram(GLuint program);
typedef GLuint WINAPI type_glGetUniformLocation(GLuint program, const GLchar *name);
typedef void WINAPI type_glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void WINAPI type_glActiveTexture(GLenum texture);
typedef void WINAPI type_glDeleteShader(GLuint shader);
typedef void WINAPI type_glDeleteProgram(GLuint program);
typedef void WINAPI type_glUniform1i(GLint location, GLint v0);
typedef void WINAPI type_glUniform1f(GLint location, GLfloat v0);
typedef void WINAPI type_glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void WINAPI type_glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef HGLRC WINAPI type_wglCreateContextAttribsARB (HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL WINAPI type_wglSwapIntervalEXT(int interval);

type_wglCreateContextAttribsARB* wglCreateContextAttribsARB;
type_wglSwapIntervalEXT* wglSwapInterval;
type_glGenVertexArrays* glGenVertexArrays;
type_glBindVertexArray* glBindVertexArray;
type_glGenBuffers* glGenBuffers;
type_glBindBuffer* glBindBuffer;
type_glBufferData* glBufferData;
type_glVertexAttribPointer* glVertexAttribPointer;
type_glEnableVertexAttribArray* glEnableVertexAttribArray;
type_glDisableVertexAttribArray* glDisableVertexAttribArray;
type_glGenerateMipmap* glGenerateMipmap;
type_glCreateShader* glCreateShader;
type_glShaderSource* glShaderSource;
type_glCompileShader* glCompileShader;
type_glGetShaderInfoLog* glGetShaderInfoLog;
type_glGetShaderiv* glGetShaderiv;
type_glCreateProgram* glCreateProgram;
type_glAttachShader* glAttachShader;
type_glLinkProgram* glLinkProgram;
type_glGetProgramiv* glGetProgramiv;
type_glUseProgram* glUseProgram;
type_glGetUniformLocation* glGetUniformLocation;
type_glUniformMatrix4fv* glUniformMatrix4fv;
type_glActiveTexture* glActiveTexture;
type_glDeleteShader* glDeleteShader;
type_glDeleteProgram* glDeleteProgram;
type_glUniform1i* glUniform1i;
type_glUniform1f* glUniform1f;
type_glUniform3f* glUniform3f;
type_glUniform4f* glUniform4f;


void InitOpenGL(HWND window){
    HDC device_context = GetDC(window);
    PIXELFORMATDESCRIPTOR descriptor = {0};
    descriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    descriptor.nVersion = 1;
    descriptor.iPixelType = PFD_TYPE_RGBA;
    descriptor.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW|PFD_DOUBLEBUFFER;
    descriptor.cColorBits = 32;
    descriptor.cAlphaBits = 8;
    descriptor.iLayerType = PFD_MAIN_PLANE;
    s32 suggested_format_index = ChoosePixelFormat(device_context, &descriptor);
    PIXELFORMATDESCRIPTOR actual_descriptor = {0};
    DescribePixelFormat(device_context, suggested_format_index, sizeof(PIXELFORMATDESCRIPTOR), &actual_descriptor);
    SetPixelFormat(device_context, suggested_format_index, &actual_descriptor);


    if(!game_state->gl_render_context){
        HGLRC render_context = wglCreateContext(device_context);

        game_state->gl_render_context = render_context;
        if(wglMakeCurrent(device_context, render_context)){
			wglCreateContextAttribsARB = (type_wglCreateContextAttribsARB*)wglGetProcAddress("wglCreateContextAttribsARB");
	
			int attrib_list[] = {
								WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
								WGL_CONTEXT_MINOR_VERSION_ARB, 2,
								WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB|WGL_CONTEXT_DEBUG_BIT_ARB,
								WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
								0
			};
			HGLRC render_context_arb = wglCreateContextAttribsARB(device_context, NULL, attrib_list);
			if(wglMakeCurrent(device_context, render_context_arb)){
			}
			wglDeleteContext(render_context);
        }
    }

    glGenVertexArrays = (type_glGenVertexArrays*)wglGetProcAddress("glGenVertexArrays");
    glBindVertexArray = (type_glBindVertexArray*)wglGetProcAddress("glBindVertexArray");
    glGenBuffers = (type_glGenBuffers*)wglGetProcAddress("glGenBuffers");
    glBindBuffer = (type_glBindBuffer*)wglGetProcAddress("glBindBuffer");
    glBufferData = (type_glBufferData*)wglGetProcAddress("glBufferData");
    glVertexAttribPointer = (type_glVertexAttribPointer*)wglGetProcAddress("glVertexAttribPointer");
    glEnableVertexAttribArray = (type_glEnableVertexAttribArray*)wglGetProcAddress("glEnableVertexAttribArray");
    glDisableVertexAttribArray = (type_glDisableVertexAttribArray*)wglGetProcAddress("glDisableVertexAttribArray");
    glGenerateMipmap = (type_glGenerateMipmap*)wglGetProcAddress("glGenerateMipmap");
    glCreateShader = (type_glCreateShader*)wglGetProcAddress("glCreateShader");
    glShaderSource = (type_glShaderSource*)wglGetProcAddress("glShaderSource");
    glCompileShader = (type_glCompileShader*)wglGetProcAddress("glCompileShader");
    glGetShaderInfoLog = (type_glGetShaderInfoLog*)wglGetProcAddress("glGetShaderInfoLog");
    glGetShaderiv = (type_glGetShaderiv*)wglGetProcAddress("glGetShaderiv");
    glCreateProgram = (type_glCreateProgram*)wglGetProcAddress("glCreateProgram");
    glAttachShader = (type_glAttachShader*)wglGetProcAddress("glAttachShader");
    glLinkProgram = (type_glLinkProgram*)wglGetProcAddress("glLinkProgram");
    glGetProgramiv = (type_glGetProgramiv*)wglGetProcAddress("glGetProgramiv");
    glUseProgram = (type_glUseProgram*)wglGetProcAddress("glUseProgram");
    glGetUniformLocation = (type_glGetUniformLocation*)wglGetProcAddress("glGetUniformLocation");
    glUniformMatrix4fv = (type_glUniformMatrix4fv*)wglGetProcAddress("glUniformMatrix4fv");
    glActiveTexture = (type_glActiveTexture*)wglGetProcAddress("glActiveTexture");
    glDeleteShader = (type_glDeleteShader*)wglGetProcAddress("glDeleteShader");
    glDeleteProgram = (type_glDeleteProgram*)wglGetProcAddress("glDeleteProgram");
    glUniform1i = (type_glUniform1i*)wglGetProcAddress("glUniform1i");
    glUniform1f = (type_glUniform1f*)wglGetProcAddress("glUniform1f");
    glUniform3f = (type_glUniform3f*)wglGetProcAddress("glUniform3f");
    glUniform4f = (type_glUniform4f*)wglGetProcAddress("glUniform4f");
    wglSwapInterval = (type_wglSwapIntervalEXT*)wglGetProcAddress("wglSwapIntervalEXT");

    glViewport(0, 0, 1280, 720);
    ReleaseDC(window, device_context);
}

#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1
#define GL_TEXTURE2 0x84C2
#define GL_TEXTURE3 0x84C3
#define GL_TEXTURE4 0x84C4
#define GL_TEXTURE5 0x84C5
#define GL_TEXTURE6 0x84C6
#define GL_TEXTURE7 0x84C7
#define GL_TEXTURE8 0x84C8
#define GL_TEXTURE9 0x84C9


