#pragma once
#include "game.h"
#include <gl/gl.h>

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
typedef void WINAPI type_glGenerateMipmap(GLenum target);
typedef GLuint WINAPI type_glCreateShader(GLenum type);
typedef void WINAPI type_glShaderSource(GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length);
typedef void WINAPI type_glCompileShader(GLuint shader);
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
// typedef void WINAPI type_glBlendFunc(GLenum sfactor, GLenum dfactor);
// typedef void WINAPI type_glPixelStorei(GLenum pname, GLuint param);
typedef BOOL WINAPI type_wglSwapIntervalEXT(int interval);

type_wglSwapIntervalEXT* wglSwapInterval;
type_glGenVertexArrays* glGenVertexArrays;
type_glBindVertexArray* glBindVertexArray;
type_glGenBuffers* glGenBuffers;
type_glBindBuffer* glBindBuffer;
type_glBufferData* glBufferData;
type_glVertexAttribPointer* glVertexAttribPointer;
type_glEnableVertexAttribArray* glEnableVertexAttribArray;
type_glGenerateMipmap* glGenerateMipmap;
type_glCreateShader* glCreateShader;
type_glShaderSource* glShaderSource;
type_glCompileShader* glCompileShader;
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
// type_glBlendFunc* glBlendFunc;
// type_glPixelStorei* glPixelStorei;

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
        }
    }

    glGenVertexArrays = (type_glGenVertexArrays*)wglGetProcAddress("glGenVertexArrays");
    glBindVertexArray = (type_glBindVertexArray*)wglGetProcAddress("glBindVertexArray");
    glGenBuffers = (type_glGenBuffers*)wglGetProcAddress("glGenBuffers");
    glBindBuffer = (type_glBindBuffer*)wglGetProcAddress("glBindBuffer");
    glBufferData = (type_glBufferData*)wglGetProcAddress("glBufferData");
    glVertexAttribPointer = (type_glVertexAttribPointer*)wglGetProcAddress("glVertexAttribPointer");
    glEnableVertexAttribArray = (type_glEnableVertexAttribArray*)wglGetProcAddress("glEnableVertexAttribArray");
    glGenerateMipmap = (type_glGenerateMipmap*)wglGetProcAddress("glGenerateMipmap");
    glCreateShader = (type_glCreateShader*)wglGetProcAddress("glCreateShader");
    glShaderSource = (type_glShaderSource*)wglGetProcAddress("glShaderSource");
    glCompileShader = (type_glCompileShader*)wglGetProcAddress("glCompileShader");
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
    // glBlendFunc = (type_glBlendFunc*)wglGetProcAddress("glBlendFunc");
    // glPixelStorei = (type_glPixelStorei*)wglGetProcAddress("glPixelStorei");
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