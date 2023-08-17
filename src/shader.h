#pragma once
#include "game.h"
#include "maths.h"
#include "mygl.h"


u32 RandomHash(char* s){
    char* at = s;
    u32 result = 1;
    while(*at != '\0'){
        result *= (*at + 4);
        result += 17;
        result *= 13;
        result /= 3;
        at++;
    }
    return result;
}

void ShaderCreate(char* vertex_shader_filepath, char* fragment_shader_filepath, GLuint* shader_program){
    File vertex_shader_file = io->ReadEntireFile(vertex_shader_filepath);
    File fragment_shader_file = io->ReadEntireFile(fragment_shader_filepath);

    if(!vertex_shader_file.memory || !fragment_shader_file.memory){
        return;
    }

    u32 valid = 1;
//    u32 difference = 0;
    int success;

//    static u32 prev_hash_vertex = 0;
//    static u32 prev_hash_fragment= 0;
//
//    static u32 cur_hash_fragment= 0;
//    cur_hash_fragment= RandomHash((char*)fragment_shader_file.memory);
//    static u32 cur_hash_vertex = 0;
//    cur_hash_vertex = RandomHash((char*)vertex_shader_file.memory);

//    cur_hash_vertex = 1;
//    cur_hash_fragment = 1;
//    prev_hash_vertex = 0;
//    prev_hash_fragment= 0;
//
//    if(cur_hash_fragment == prev_hash_fragment && cur_hash_vertex == prev_hash_vertex){
//        io->FreeEntireFile(fragment_shader_file);
//        io->FreeEntireFile(vertex_shader_file);
//        prev_hash_fragment = cur_hash_fragment;
//        prev_hash_vertex = cur_hash_vertex;
//        return;
//    }
//    prev_hash_fragment = cur_hash_fragment;
//    prev_hash_vertex = cur_hash_vertex;

    GLuint vertex_shader;
    GLuint fragment_shader;
//    prev_hash_vertex = cur_hash_vertex;
//    difference = 1;
//    prev_hash_vertex = cur_hash_vertex;
    const char* vertex_shader_source = (char*)vertex_shader_file.memory;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success){
        valid = 0;
    }

//    prev_hash_fragment = cur_hash_fragment;
//    difference = 1;
//    prev_hash_fragment = cur_hash_fragment;
    const char* fragment_shader_source = (char*)fragment_shader_file.memory;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success){
        valid = 0;
    }

    if(*shader_program){
        glDeleteProgram(*shader_program);
    }
    *shader_program = glCreateProgram();
    glAttachShader(*shader_program, vertex_shader);
    glAttachShader(*shader_program, fragment_shader);
    glLinkProgram(*shader_program);
    glGetProgramiv(*shader_program, GL_LINK_STATUS, &success);
    if(!success){
        valid = 0;
    }
    glDeleteShader(fragment_shader);
    glDeleteShader(vertex_shader);
    io->FreeEntireFile(vertex_shader_file);
    io->FreeEntireFile(fragment_shader_file);

    if(!valid){
        glDeleteProgram(*shader_program);
        *shader_program = 0;
    }
    else{
    }
}

void ShaderSetUniform(unsigned int shader_program, char* uniform_name, int value){
    int uniform_id = glGetUniformLocation(shader_program, uniform_name);
    glUseProgram(shader_program);
    glUniform1i(uniform_id, value);
}

void ShaderSetUniform(unsigned int shader_program, char* uniform_name, float value){
    int uniform_id = glGetUniformLocation(shader_program, uniform_name);
    glUseProgram(shader_program);
    glUniform1f(uniform_id, value);
}

void ShaderSetUniform(unsigned int shader_program, char* uniform_name, v3 value){
    int uniform_id = glGetUniformLocation(shader_program, uniform_name);
    glUseProgram(shader_program);
    glUniform3f(uniform_id, value.x, value.y, value.z);
}

void ShaderSetUniform(unsigned int shader_program, char* uniform_name, v4 value){
    int uniform_id = glGetUniformLocation(shader_program, uniform_name);
    glUseProgram(shader_program);
    glUniform4f(uniform_id, value.x, value.y, value.z, value.w);
}

void ShaderSetUniform(unsigned int shader_program, char* uniform_name, mat4 value){
    int uniform_id = glGetUniformLocation(shader_program, uniform_name);
    glUseProgram(shader_program);
    glUniformMatrix4fv(uniform_id, 1, false, (GLfloat*)&value);
}