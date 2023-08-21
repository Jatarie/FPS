#include "game.h"
#include "mygl.h"
#include "shader.h"
#include "maths.h"
#include <assimp/ai_assert.h>
#include "DataStructures.h"

/*Questions:
    1. What determines what is drawn by glDrawArrays?
    2. What is the use case for a VAO.
    3. How do you associate VBO data with a VAO.
    4. What does glBindBuffer do? 
    5. What does it do in relation to glBufferData?
    6. How does specular lighting actually work? How can you run the shader once per fragment and end up with something that looks like that.
    7. What is going on with the diffuse map calculation in the fragment shader. Is it supposed to be in radians or degrees? Radians seems like its better.
*/

 void TestDrawTriangle(HWND window){

     float vertices[] = {
         0.0f, 0.5f, 0.0f,  // top
         -0.5f, -0.5f, 0.0f, // bottom left
         0.5f, -0.5f, 0.0f   // bottom right
     };

     static u32 init = 0;
     static GLuint vbo;
     static GLuint vao;
     if(!init){
         init = 1;

         glGenVertexArrays(1, &vao);
         glBindVertexArray(vao);

         glGenBuffers(1, &vbo);
         glBindBuffer(GL_ARRAY_BUFFER, vbo);
         glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
         glEnableVertexAttribArray(0);
         glBindBuffer(GL_ARRAY_BUFFER, 0);

         glBindVertexArray(0);

     }

     glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
     glClear(GL_COLOR_BUFFER_BIT);

     glBindVertexArray(vao);
     glDrawArrays(GL_TRIANGLES, 0, 3);

     HDC device_context = GetDC(window);
     SwapBuffers(device_context);
     ReleaseDC(window, device_context);
 }

 void TestDrawTexture(HWND window){
     float vertices[] = {
         0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
     };

     static u32 init = 0;
     static GLuint vbo;
     static GLuint vao;
     static GLuint texture;
     static GLuint shader_program;
     if(!init){
         init = 1;

         ShaderCreate("c:/code/fps/src/shaders/texture_test_vertex.hlsl", "c:/code/fps/src/shaders/texture_test_fragment.hlsl", &shader_program);

         // Load the image file into memory
         int width, height, nrChannels;
         unsigned char *data = stbi_load("c:/code/FPS/build/sensors.png", &width, &height, &nrChannels, 0);

         // Generate a texture object
         glGenTextures(1, &texture);
         glBindTexture(GL_TEXTURE_2D, texture);

         // Set texture parameters
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

         // Copy the image data to the texture object
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

         // Generate mipmaps for the texture
         glGenerateMipmap(GL_TEXTURE_2D);

         // Free the image data from memory
         stbi_image_free(data);

         glGenVertexArrays(1, &vao);

         glBindVertexArray(vao);
             glGenBuffers(1, &vbo);
             glBindBuffer(GL_ARRAY_BUFFER, vbo);
                 glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                 glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
                 glEnableVertexAttribArray(0);

                 glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
                 glEnableVertexAttribArray(1);
             glBindBuffer(GL_ARRAY_BUFFER, 0);
         glBindVertexArray(0);

     }


     glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
     glClear(GL_COLOR_BUFFER_BIT);

     //glUseProgram before glBindVertexArray so that geometry is drawn correctly
     glUseProgram(shader_program);
     glBindVertexArray(vao);

     glDrawArrays(GL_TRIANGLES, 0, 3);

     HDC device_context = GetDC(window);
     SwapBuffers(device_context);
     ReleaseDC(window, device_context);

 }

 void TestEBO(HWND window){
     float vertices[] = {
         0.5f, 0.5f, 0.0f, //TR
         0.5f, -0.5f, 0.0f, //BR
         -0.5f, -0.5f, 0.0f, //BL
         -0.5f, 0.5f, 0.0f // TL
     };

     u32 indices[] = {
         0, 1, 2,
         2, 3, 0
     };

     static u32 init = 0;
     static GLuint vbo;
     static GLuint vao;
     static GLuint ebo;
     if(!init){
         init = 1;

         glGenVertexArrays(1, &vao);
         glBindVertexArray(vao);

         glGenBuffers(1, &ebo);
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
         glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

         glGenBuffers(1, &vbo);
         glBindBuffer(GL_ARRAY_BUFFER, vbo);
         glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
         glEnableVertexAttribArray(0);

         glBindBuffer(GL_ARRAY_BUFFER, 0);
         glBindVertexArray(0);

     }

     glClearColor(0.2f, 0.2f, 0.0f, 1.0f);
     glClear(GL_COLOR_BUFFER_BIT);

     glBindVertexArray(vao);
     glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

     HDC device_context = GetDC(window);
     SwapBuffers(device_context);
     ReleaseDC(window, device_context);
 }


 void TestDrawCube(HWND window){
     float cube_vertices[] = { -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, -0.5f, 0.5f, -0.5f, 0.0f, 1.0f };

     static u32 init = 0;
     static GLuint vbo;
     static GLuint vao;
     static GLuint shader_program;
     if(!init){
         init = 1;
         ShaderCreate("c:/code/fps/src/shaders/test_cube_vertex.hlsl", "c:/code/fps/src/shaders/test_cube_fragment.hlsl", &shader_program);

         glGenVertexArrays(1, &vao);
         glBindVertexArray(vao);

         glGenBuffers(1, &vbo);
         glBindBuffer(GL_ARRAY_BUFFER, vbo);
         glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
         glEnableVertexAttribArray(0);

         glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
         glEnableVertexAttribArray(1);
         glBindBuffer(GL_ARRAY_BUFFER, 0);

         glBindVertexArray(0);

     }

     static r32 rot = 0;
     rot++;

     mat4 scale = CreateScaleMatrix({1.0, 1.0, 1.0});
     mat4 rotation = CreateRotationMatrix(Normalize({1.0f, 1.0f, 1.0f}), rot);
     mat4 translation = CreateTranslationMatrix({0, 1, 1.5f});
     mat4 model = scale * rotation * translation;

     mat4 projection = CreatePerspectiveMatrix(ToRadians(120.0f), 0.01f, 1000.0f, 1280.0f, 720.0f);

     mat4 view = LookAt(game_state->cam.world_p, game_state->cam.camera_dir+ game_state->cam.world_p);

     ShaderSetUniform(shader_program, "model", model);
     ShaderSetUniform(shader_program, "view", view);
     ShaderSetUniform(shader_program, "projection", projection);

     glEnable(GL_DEPTH_TEST);
     glClearColor(0.2f, 0.2f, 0.0f, 1.0f);
     glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

     glBindVertexArray(vao);
     glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_vertices));

     HDC device_context = GetDC(window);
     SwapBuffers(device_context);
     ReleaseDC(window, device_context);

 }

 void TestLighting(HWND window){
     float cube_vertices[] = {
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

     static u32 init = 0;
     static GLuint vbo;
     static GLuint vao;
     static GLuint light_vao;
     static GLuint cube_shader;
     static GLuint lighting_cube_shader;
     if(!init){
         init = 1;
         ShaderCreate("c:/code/fps/src/shaders/lighting_tests/test_lighting_vertex.hlsl", "c:/code/fps/src/shaders/test_lighting_fragment.hlsl", &cube_shader);
		 ShaderCreate("c:/code/fps/src/shaders/lighting_tests/test_lighting_cube_vertex.hlsl", "c:/code/fps/src/shaders/test_lighting_cube_fragment.hlsl", &lighting_cube_shader);

         glGenVertexArrays(1, &vao);
         glGenVertexArrays(1, &light_vao);
         glBindVertexArray(vao);

         glGenBuffers(1, &vbo);
         glBindBuffer(GL_ARRAY_BUFFER, vbo);
         glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
         glEnableVertexAttribArray(0);

         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
         glEnableVertexAttribArray(1);
        
         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
         glEnableVertexAttribArray(2);
         glBindBuffer(GL_ARRAY_BUFFER, 0);

         glBindVertexArray(0);


         glBindVertexArray(light_vao);

         glBindBuffer(GL_ARRAY_BUFFER, vbo);
         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
         glEnableVertexAttribArray(0);

         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
         glEnableVertexAttribArray(1);
        
         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
         glEnableVertexAttribArray(2);
         glBindBuffer(GL_ARRAY_BUFFER, 0);

         glBindVertexArray(0);

     }

     static u32 rot = 0;
     rot++;

     v3 object_color = {0.2f, 0.4f, 1.0f};

     mat4 scale = CreateScaleMatrix({1.0f, 1.0f, 1.0f});
     mat4 rotation = CreateRotationMatrix(Normalize({0.0f, 0.0f, 1.0f}), 0);
     mat4 translation = CreateTranslationMatrix({0, 0, 0});
     mat4 model = scale * Identity() * translation;

     mat4 projection = CreatePerspectiveMatrix(ToRadians(90.0f), 0.01f, 1000.0f, 1280.0f, 720.0f);

     mat4 view = LookAt(game_state->cam.world_p, game_state->cam.camera_dir+ game_state->cam.world_p);


     v3 light_color = {1, 1, 1};

     ShaderSetUniform(lighting_cube_shader, "model", model);
     ShaderSetUniform(lighting_cube_shader, "view", view);
     ShaderSetUniform(lighting_cube_shader, "projection", projection);

     ShaderSetUniform(lighting_cube_shader, "light_color", light_color);

     glEnable(GL_DEPTH_TEST);
     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
     glClearColor(0.5f, 0.2f, 0.2f, 1.0f);
     glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

     glBindVertexArray(light_vao);
     glUseProgram(lighting_cube_shader);
     glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_vertices));

     translation = CreateTranslationMatrix({3.5, -2, 0});
     model = scale * rotation * translation;

     ShaderSetUniform(cube_shader, "model", model);
     ShaderSetUniform(cube_shader, "view", view);
     ShaderSetUniform(cube_shader, "projection", projection);
     ShaderSetUniform(cube_shader, "camera_pos", game_state->cam.world_p);

     ShaderSetUniform(cube_shader, "material.ambient", v3{1.0f, 0.5f, 0.31f});
     ShaderSetUniform(cube_shader, "material.diffuse", v3{1.0f, 0.5f, 0.31f});
     ShaderSetUniform(cube_shader, "material.specular", v3{0.5f, 0.5f, 0.5f});
     ShaderSetUniform(cube_shader, "material.shininess", 32.0f);

     ShaderSetUniform(cube_shader, "light.position", v3{0.0f, 0.0f, 0.0f});
     ShaderSetUniform(cube_shader, "light.ambient", v3{0.1f, 0.1f, 0.1f} * light_color);
     ShaderSetUniform(cube_shader, "light.diffuse", v3{1.0f, 1.0f, 1.0f} * light_color);
     ShaderSetUniform(cube_shader, "light.specular", v3{1.0f, 1.0f, 1.0f} * light_color);

     glBindVertexArray(vao);
     glUseProgram(cube_shader);
     glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_vertices));

     HDC device_context = GetDC(window);
     SwapBuffers(device_context);
     ReleaseDC(window, device_context);
 }

 void TestLightingMaps(HWND window){
     float cube_vertices[] = {
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

     static u32 init = 0;
     static GLuint vbo;
     static GLuint vao;
     static GLuint light_vao;
     static GLuint cube_shader;
     static GLuint lighting_cube_shader;
     static GLuint texture;
     static GLuint specular_map;
     static GLuint emission_map;
     if(!init){
         init = 1;
         ShaderCreate("c:/code/fps/src/shaders/lighting_tests/test_lighting_vertex2.hlsl", "c:/code/fps/src/shaders/lighting_tests/test_lighting_fragment2.hlsl", &cube_shader);
         ShaderCreate("c:/code/fps/src/shaders/lighting_tests/test_lighting_cube_vertex2.hlsl", "c:/code/fps/src/shaders/lighting_tests/test_lighting_cube_fragment2.hlsl", &lighting_cube_shader);

         specular_map = LoadTexture("c:/code/FPS/src/specular_map.bmp");
         texture = LoadTexture("c:/code/FPS/src/box.bmp");
         emission_map = LoadTexture("c:/code/FPS/src/emission_map.bmp");

         glGenVertexArrays(1, &vao);
         glGenVertexArrays(1, &light_vao);
         glBindVertexArray(vao);

         glGenBuffers(1, &vbo);
         glBindBuffer(GL_ARRAY_BUFFER, vbo);
         glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
         glEnableVertexAttribArray(0);

         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
         glEnableVertexAttribArray(1);
        
         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
         glEnableVertexAttribArray(2);
         glBindBuffer(GL_ARRAY_BUFFER, 0);

         glBindVertexArray(0);


         glBindVertexArray(light_vao);

         glBindBuffer(GL_ARRAY_BUFFER, vbo);
         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
         glEnableVertexAttribArray(0);

         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
         glEnableVertexAttribArray(1);
        
         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
         glEnableVertexAttribArray(2);
         glBindBuffer(GL_ARRAY_BUFFER, 0);

         glBindVertexArray(0);

     }

     static u32 rot = 0;
     rot++;

     v3 object_color = {0.2f, 0.4f, 1.0f};
     v3 light_pos = {1.2f, 1.0f, 2.0f};

     mat4 scale = CreateScaleMatrix({1.0f, 1.0f, 1.0f});
     mat4 rotation = CreateRotationMatrix(Normalize({0.0f, 0.0f, 1.0f}), 0);
     mat4 translation = CreateTranslationMatrix(light_pos);
     mat4 model = scale * Identity() * translation;

     mat4 projection = CreatePerspectiveMatrix(ToRadians(90.0f), 0.01f, 1000.0f, 1280.0f, 720.0f);

     mat4 view = LookAt(game_state->cam.world_p, game_state->cam.camera_dir + game_state->cam.world_p);


     v3 light_color = {1, 1, 1};

     ShaderSetUniform(lighting_cube_shader, "model", model);
     ShaderSetUniform(lighting_cube_shader, "view", view);
     ShaderSetUniform(lighting_cube_shader, "projection", projection);

     ShaderSetUniform(lighting_cube_shader, "light_color", light_color);

     glEnable(GL_DEPTH_TEST);
     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
     glClearColor(0.5f, 0.2f, 0.2f, 1.0f);
     glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

     glBindVertexArray(light_vao);
     glUseProgram(lighting_cube_shader);
     glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_vertices));

     glBindVertexArray(vao);
     glUseProgram(cube_shader);

     ShaderSetUniform(cube_shader, "material.diffuse", 0);
     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D, texture);

     ShaderSetUniform(cube_shader, "material.specular", 1);
     glActiveTexture(GL_TEXTURE1);
     glBindTexture(GL_TEXTURE_2D, specular_map);

     ShaderSetUniform(cube_shader, "material.emission", 2);
     glActiveTexture(GL_TEXTURE2);
     glBindTexture(GL_TEXTURE_2D, emission_map);

     translation = CreateTranslationMatrix({0, 0, 0});
     model = scale * rotation * translation;

     ShaderSetUniform(cube_shader, "model", model);
     ShaderSetUniform(cube_shader, "view", view);
     ShaderSetUniform(cube_shader, "projection", projection);
     ShaderSetUniform(cube_shader, "camera_pos", game_state->cam.world_p);

     ShaderSetUniform(cube_shader, "material.shininess", 32.0f);

     ShaderSetUniform(cube_shader, "light.position", light_pos);
     ShaderSetUniform(cube_shader, "light.ambient", v3{0.2f, 0.2f, 0.2f} * light_color);
     ShaderSetUniform(cube_shader, "light.diffuse", v3{1.0f, 1.0f, 1.0f} * light_color);
     ShaderSetUniform(cube_shader, "light.specular", v3{1.0f, 1.0f, 1.0f} * light_color);

     glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_vertices));

     HDC device_context = GetDC(window);
     SwapBuffers(device_context);
     ReleaseDC(window, device_context);
 }

 void TestLightTypes(HWND window){
     float cube_vertices[] = {
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

     static u32 init = 0;
     static GLuint vbo;
     static GLuint vao;
     static GLuint light_vao;
     static GLuint cube_shader;
     static GLuint lighting_cube_shader;
     static GLuint texture;
     static GLuint specular_map;
     static GLuint emission_map;
     if(!init){
         init = 1;
         ShaderCreate("c:/code/fps/src/shaders/lighting_tests/test_lighting_vertex2.hlsl", "c:/code/fps/src/shaders/lighting_tests/test_lighting_fragment3.hlsl", &cube_shader);
         ShaderCreate("c:/code/fps/src/shaders/lighting_tests/test_lighting_cube_vertex2.hlsl", "c:/code/fps/src/shaders/lighting_tests/test_lighting_cube_fragment2.hlsl", &lighting_cube_shader);

         specular_map = LoadTexture("c:/code/FPS/src/specular_map.bmp");
         texture = LoadTexture("c:/code/FPS/src/box.bmp");
         emission_map = LoadTexture("c:/code/FPS/src/emission_map.bmp");

         glGenVertexArrays(1, &vao);
         glGenVertexArrays(1, &light_vao);
         glBindVertexArray(vao);

         glGenBuffers(1, &vbo);
         glBindBuffer(GL_ARRAY_BUFFER, vbo);
         glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
         glEnableVertexAttribArray(0);

         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
         glEnableVertexAttribArray(1);
        
         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
         glEnableVertexAttribArray(2);
         glBindBuffer(GL_ARRAY_BUFFER, 0);

         glBindVertexArray(0);


         glBindVertexArray(light_vao);

         glBindBuffer(GL_ARRAY_BUFFER, vbo);
         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
         glEnableVertexAttribArray(0);

         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
         glEnableVertexAttribArray(1);
        
         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
         glEnableVertexAttribArray(2);
         glBindBuffer(GL_ARRAY_BUFFER, 0);

         glBindVertexArray(0);

     }

     static u32 rot = 0;
     rot++;

     v3 object_color = {0.2f, 0.4f, 1.0f};
     v3 light_pos = {0.0f, 0.0f, 1.0f};

     mat4 scale = CreateScaleMatrix({1.0f, 1.0f, 1.0f});
     mat4 rotation = CreateRotationMatrix(Normalize({0.0f, 0.0f, 1.0f}), 0);
     mat4 translation = CreateTranslationMatrix(light_pos);
     mat4 model = scale * Identity() * translation;

     mat4 projection = CreatePerspectiveMatrix(ToRadians(90.0f), 0.01f, 1000.0f, 1280.0f, 720.0f);

     mat4 view = LookAt(game_state->cam.world_p, game_state->cam.camera_dir + game_state->cam.world_p);


     v3 light_color = {1, 1, 1};

     ShaderSetUniform(lighting_cube_shader, "model", model);
     ShaderSetUniform(lighting_cube_shader, "view", view);
     ShaderSetUniform(lighting_cube_shader, "projection", projection);


     ShaderSetUniform(lighting_cube_shader, "light_color", light_color);

     glEnable(GL_DEPTH_TEST);
     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
     glClearColor(0.5f, 0.2f, 0.2f, 1.0f);
     glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

     glBindVertexArray(light_vao);
     glUseProgram(lighting_cube_shader);
     glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_vertices));

     glBindVertexArray(vao);
     glUseProgram(cube_shader);

     ShaderSetUniform(cube_shader, "material.diffuse", 0);
     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D, texture);

     ShaderSetUniform(cube_shader, "material.specular", 1);
     glActiveTexture(GL_TEXTURE1);
     glBindTexture(GL_TEXTURE_2D, specular_map);

     ShaderSetUniform(cube_shader, "material.emission", 2);
     glActiveTexture(GL_TEXTURE2);
     glBindTexture(GL_TEXTURE_2D, emission_map);

     ShaderSetUniform(cube_shader, "light.constant", 1.0f);
     ShaderSetUniform(cube_shader, "light.linear", 0.07f);
     ShaderSetUniform(cube_shader, "light.quadratic", 0.017f);

     translation = CreateTranslationMatrix({0, 0, 0});
     model = scale * rotation * translation;

     ShaderSetUniform(cube_shader, "view", view);
     ShaderSetUniform(cube_shader, "projection", projection);
     ShaderSetUniform(cube_shader, "camera_pos", game_state->camera_p);

     ShaderSetUniform(cube_shader, "material.shininess", 32.0f);

     ShaderSetUniform(cube_shader, "light.cutoff", cosf(ToRadians(12.5f)));
     ShaderSetUniform(cube_shader, "light.outercutoff", cosf(ToRadians(17.5f)));
     ShaderSetUniform(cube_shader, "light.direction", game_state->camera_dir);
     ShaderSetUniform(cube_shader, "light.position", game_state->camera_p);
     ShaderSetUniform(cube_shader, "light.ambient", v3{0.2f, 0.2f, 0.2f} * light_color);
     ShaderSetUniform(cube_shader, "light.diffuse", v3{1.0f, 1.0f, 1.0f} * light_color);
     ShaderSetUniform(cube_shader, "light.specular", v3{1.0f, 1.0f, 1.0f} * light_color);

     srand(0);

     for(int i = 0; i < 20; i ++){
         s32 x = (rand() % 11) - 5;
         s32 y = (rand() % 11) - 5;
         s32 z = (rand() % 11) - 5;

         r32 xr = (r32)(rand() % 360) / 360.0f;
         r32 yr = (r32)(rand() % 360) / 360.0f;
         r32 zr = (r32)(rand() % 360) / 360.0f;

         r32 deg = (r32)(rand() % 360);

         rotation = CreateRotationMatrix(Normalize({xr, yr, zr}), deg);
         translation = CreateTranslationMatrix(v3{(r32)x, (r32)y, (r32)z});
         // translation = CreateTranslationMatrix(v3{(r32)i+1, (r32)i, (r32)i});
         model = scale * Identity() * translation;
         ShaderSetUniform(cube_shader, "model", model);
         glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_vertices));
     }

     HDC device_context = GetDC(window);
     SwapBuffers(device_context);
     ReleaseDC(window, device_context);
 }

 struct Point_Light{
     v3 position;
     v3 light_color;

     v3 ambient;
     v3 diffuse;
     v3 specular;

     float constant;
     float linear;
     float quadratic;
 };

 void AddPointLight(GLuint shader, Point_Light l, u32 index){
     char buffer[255];
     sprintf(buffer, "point_light[%d].position", index);
     ShaderSetUniform(shader, buffer, l.position);
     sprintf(buffer, "point_light[%d].ambient", index);
     ShaderSetUniform(shader, buffer, l.ambient * l.light_color);
     sprintf(buffer, "point_light[%d].diffuse", index);
     ShaderSetUniform(shader, buffer, l.diffuse * l.light_color);
     sprintf(buffer, "point_light[%d].specular", index);
     ShaderSetUniform(shader, buffer, l.specular * l.light_color);

     sprintf(buffer, "point_light[%d].constant", index);
     ShaderSetUniform(shader, buffer, l.constant);
     sprintf(buffer, "point_light[%d].linear", index);
     ShaderSetUniform(shader, buffer, l.linear);
     sprintf(buffer, "point_light[%d].quadratic", index);
     ShaderSetUniform(shader, buffer, l.quadratic);
 }

 void TestManyLights(HWND window){
     float cube_vertices[] = {
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

     static u32 init = 0;
     static GLuint vbo;
     static GLuint vao;
     static GLuint light_vao;
     static GLuint cube_shader;
     static GLuint lighting_cube_shader;
     static GLuint texture;
     static GLuint specular_map;
     static GLuint emission_map;
     if(!init){
         init = 1;
         ShaderCreate("c:/code/fps/src/shaders/lighting_tests/test_lighting_vertex2.hlsl", "c:/code/fps/src/shaders/lighting_tests/test_lighting_fragmentmultiple.hlsl", &cube_shader);
         ShaderCreate("c:/code/fps/src/shaders/lighting_tests/test_lighting_cube_vertex2.hlsl", "c:/code/fps/src/shaders/lighting_tests/test_lighting_cube_fragment2.hlsl", &lighting_cube_shader);

         specular_map = LoadTexture("c:/code/FPS/src/specular_map.bmp");
         texture = LoadTexture("c:/code/FPS/src/box.bmp");
         emission_map = LoadTexture("c:/code/FPS/src/emission_map.bmp");

         glGenVertexArrays(1, &vao);
         glGenVertexArrays(1, &light_vao);
         glBindVertexArray(vao);

         glGenBuffers(1, &vbo);
         glBindBuffer(GL_ARRAY_BUFFER, vbo);
         glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
         glEnableVertexAttribArray(0);

         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
         glEnableVertexAttribArray(1);
        
         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
         glEnableVertexAttribArray(2);
         glBindBuffer(GL_ARRAY_BUFFER, 0);

         glBindVertexArray(0);


         glBindVertexArray(light_vao);

         glBindBuffer(GL_ARRAY_BUFFER, vbo);
         glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
         glEnableVertexAttribArray(0);

         glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
         glEnableVertexAttribArray(1);
        
         glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
         glEnableVertexAttribArray(2);
         glBindBuffer(GL_ARRAY_BUFFER, 0);

         glBindVertexArray(0);

     }

     static u32 rot = 0;
     rot++;

     v3 object_color = {0.2f, 0.4f, 1.0f};
     v3 light_pos = {0.0f, 0.0f, 1.0f};

     mat4 scale = CreateScaleMatrix({1.0f, 1.0f, 1.0f});
     mat4 rotation = CreateRotationMatrix(Normalize({0.0f, 0.0f, 1.0f}), 0);
     mat4 translation = CreateTranslationMatrix(light_pos);
     mat4 model = scale * Identity() * translation;

     mat4 projection = CreatePerspectiveMatrix(ToRadians(90.0f), 0.01f, 1000.0f, 1280.0f, 720.0f);

     mat4 view = LookAt(game_state->camera_p, game_state->camera_dir + game_state->camera_p);

     glEnable(GL_DEPTH_TEST);
     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
     glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
     glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  
     ShaderSetUniform(cube_shader, "material.diffuse", 0);
     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D, texture);

     ShaderSetUniform(cube_shader, "material.specular", 1);
     glActiveTexture(GL_TEXTURE1);
     glBindTexture(GL_TEXTURE_2D, specular_map);

     ShaderSetUniform(cube_shader, "material.emission", 2);
     glActiveTexture(GL_TEXTURE2);
     glBindTexture(GL_TEXTURE_2D, emission_map);

     translation = CreateTranslationMatrix({0, 0, 0});
     model = scale * rotation * translation;

     ShaderSetUniform(cube_shader, "view", view);
     ShaderSetUniform(cube_shader, "projection", projection);
     ShaderSetUniform(cube_shader, "camera_pos", game_state->camera_p);

     ShaderSetUniform(cube_shader, "material.shininess", 32.0f);

     // ShaderSetUniform(cube_shader, "light.cutoff", cos(ToRadians(12.5f)));
     // ShaderSetUniform(cube_shader, "light.outercutoff", cos(ToRadians(17.5f)));

     v3 light_color = v3{1, 1, 1};

     ShaderSetUniform(cube_shader, "directional_light.direction", v3{1.0f, 6.0f, 1.0f});
     ShaderSetUniform(cube_shader, "directional_light.ambient", v3{0.2f, 0.2f, 0.2f} * light_color);
     ShaderSetUniform(cube_shader, "directional_light.diffuse", v3{1.0f, 1.0f, 1.0f} * light_color);
     ShaderSetUniform(cube_shader, "directional_light.specular", v3{1.0f, 1.0f, 1.0f} * light_color);

     ShaderSetUniform(cube_shader, "spot_light.direction", game_state->camera_dir);
     ShaderSetUniform(cube_shader, "spot_light.position", game_state->camera_p);
     ShaderSetUniform(cube_shader, "spot_light.ambient", v3{0.01f, 0.01f, 0.01f} * light_color);
     ShaderSetUniform(cube_shader, "spot_light.diffuse", v3{1.0f, 1.0f, 1.0f} * light_color);
     ShaderSetUniform(cube_shader, "spot_light.specular", v3{1.0f, 1.0f, 1.0f} * light_color);
	 ShaderSetUniform(cube_shader, "spot_light.cutoff", cosf(ToRadians(0.0f)));
     ShaderSetUniform(cube_shader, "spot_light.outercutoff", cosf(ToRadians(30.0f)));

     ShaderSetUniform(cube_shader, "spot_light.constant", 1.0f);
     ShaderSetUniform(cube_shader, "spot_light.linear", 0.022f);
     ShaderSetUniform(cube_shader, "spot_light.quadratic", 0.0019f);


     Point_Light p0;
     p0.position = v3{0.0f, 0.0f, 0.0f};
     p0.light_color = v3{1.0f, 0.0f, 0.0f};
     p0.ambient = v3{0.2f, 0.2f, 0.2f};
     p0.diffuse = v3{1.0f, 1.0f, 1.0f};
     p0.specular = v3{1.0f, 1.0f, 1.0f};
     p0.constant = 1.0f;
     p0.linear = 0.07f;
     p0.quadratic = 0.017f;

     Point_Light p1;
     p1.position = v3{8.0f, 0.0f, 0.0f};
     p1.light_color = v3{1.0f, 0.0f, 0.0f};
     p1.ambient = v3{0.2f, 0.2f, 0.2f};
     p1.diffuse = v3{1.0f, 1.0f, 1.0f};
     p1.specular = v3{1.0f, 1.0f, 1.0f};
     p1.constant = 1.0f;
     p1.linear = 0.07f;
     p1.quadratic = 0.017f;

     Point_Light p2;
     p2.position = v3{0.0f, 12.0f, 2.0f};
     p2.light_color = v3{0.0f, 1.0f, 0.0f};
     p2.ambient = v3{0.2f, 0.2f, 0.2f};
     p2.diffuse = v3{1.0f, 1.0f, 1.0f};
     p2.specular = v3{1.0f, 1.0f, 1.0f};
     p2.constant = 1.0f;
     p2.linear = 0.07f;
     p2.quadratic = 0.017f;

     Point_Light p3;
     p3.position = v3{3.0f, -6.0f, 3.0f};
     p3.light_color = v3{0.0f, 1.0f, 0.0f};
     p3.ambient = v3{0.2f, 0.2f, 0.2f};
     p3.diffuse = v3{1.0f, 1.0f, 1.0f};
     p3.specular = v3{1.0f, 1.0f, 1.0f};
     p3.constant = 1.0f;
     p3.linear = 0.07f;
     p3.quadratic = 0.017f;

     glBindVertexArray(light_vao);
     ShaderSetUniform(lighting_cube_shader, "view", view);
     ShaderSetUniform(lighting_cube_shader, "projection", projection);

     model = scale * CreateTranslationMatrix(p0.position);
     ShaderSetUniform(lighting_cube_shader, "light_color", p0.light_color);
     ShaderSetUniform(lighting_cube_shader, "model", model);
     AddPointLight(cube_shader, p0, 0);
     glUseProgram(lighting_cube_shader);
     glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_vertices));

     model = scale * CreateTranslationMatrix(p1.position);
     ShaderSetUniform(lighting_cube_shader, "light_color", p1.light_color);
     ShaderSetUniform(lighting_cube_shader, "model", model);
     AddPointLight(cube_shader ,p1, 1);
     glUseProgram(lighting_cube_shader);
     glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_vertices));

     model = scale * CreateTranslationMatrix(p2.position);
     ShaderSetUniform(lighting_cube_shader, "light_color", p2.light_color);
     ShaderSetUniform(lighting_cube_shader, "model", model);
     AddPointLight(cube_shader ,p2, 2);
     glUseProgram(lighting_cube_shader);
     glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_vertices));

     model = scale * CreateTranslationMatrix(p3.position);
     ShaderSetUniform(lighting_cube_shader, "light_color", p3.light_color);
     ShaderSetUniform(lighting_cube_shader, "model", model);
     AddPointLight(cube_shader ,p3, 3);
     glUseProgram(lighting_cube_shader);
     glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_vertices));

     srand(0);

     glBindVertexArray(vao);
     glUseProgram(cube_shader);
     for(int i = 0; i < 20; i ++){
         s32 x = (rand() % 11) - 5;
         s32 y = (rand() % 11) - 5;
         s32 z = (rand() % 11) - 5;

         r32 xr = (r32)(rand() % 360) / 360.0f;
         r32 yr = (r32)(rand() % 360) / 360.0f;
         r32 zr = (r32)(rand() % 360) / 360.0f;

         r32 deg = (r32)(rand() % 360);

         rotation = CreateRotationMatrix(Normalize({xr, yr, zr}), deg);
         translation = CreateTranslationMatrix(v3{(r32)x, (r32)y, (r32)z});
         model = scale * Identity() * translation;
         ShaderSetUniform(cube_shader, "model", model);
         glDrawArrays(GL_TRIANGLES, 0, sizeof(cube_vertices));
     }

     HDC device_context = GetDC(window);
     SwapBuffers(device_context);
     ReleaseDC(window, device_context);
 }