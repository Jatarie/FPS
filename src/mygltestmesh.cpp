#include "game.h"
#include "mygl.h"
#include "shader.h"
#include "maths.h"
#include "random"
#include "DataStructures.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

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

struct Mesh{
    Material material;

    u32 vertex_count;
    Vertex* vertices;

    u32 index_count;
    u32* indices;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    GLuint shader;
};


struct Model{
    u32 mesh_count;
    Mesh** meshes;
};


Mesh* LoadMesh(aiMesh* ai_mesh, mat4 transform){
    Mesh* mesh = (Mesh*)Malloc(sizeof(Mesh));
    u32 num_vertices = ai_mesh->mNumVertices;
    mesh->vertex_count = num_vertices;

    u32 num_faces = ai_mesh->mNumFaces;

    Vertex* vertex_data = (Vertex*)Malloc(sizeof(Vertex) * num_vertices);
    u32* index_data = (u32*)Malloc(sizeof(u32) * num_faces * 3);
    mesh->index_count = num_faces*3;

    glm::mat3 normalMatrix = glm::transpose(glm::inverseTranspose(glm::mat3(*reinterpret_cast<glm::mat4*>((void*)&transform))));

    for(int i = 0; i < num_vertices; i ++){
        Vertex* vertex = &vertex_data[i];
        v3 position;
        position.x = (ai_mesh->mVertices[i]).x;
        position.y = (ai_mesh->mVertices[i]).y;
        position.z = (ai_mesh->mVertices[i]).z;

        v3 normal;
        normal.x = (ai_mesh->mNormals[i]).x;
        normal.y = (ai_mesh->mNormals[i]).y;
        normal.z = (ai_mesh->mNormals[i]).z;

        v2 texture_coords;
        texture_coords.x = (ai_mesh->mTextureCoords[0][i]).x;
        texture_coords.y = (ai_mesh->mTextureCoords[0][i]).y;

        vertex->position = transform * position;

        glm::vec3 glm_normal;
        glm_normal.x = normal.x;
        glm_normal.y = normal.y;
        glm_normal.z = normal.z;
        glm_normal = normalMatrix * glm_normal;
        vertex->normal.x = glm_normal.x;
        vertex->normal.y = glm_normal.y;
        vertex->normal.z = glm_normal.z;
        vertex->texture_coords = texture_coords;
        
    }

    for(int i = 0; i < num_faces; i ++){
        index_data[i*3 + 0] = ai_mesh->mFaces[i].mIndices[0];
        index_data[i*3 + 1] = ai_mesh->mFaces[i].mIndices[1];
        index_data[i*3 + 2] = ai_mesh->mFaces[i].mIndices[2];
    }
    mesh->indices = index_data;
    mesh->vertices = vertex_data;

    GLuint albedo = LoadTexture("c:/code/FPS/src/models/guitar/textures/1001_albedo.jpg");
    GLuint specular = LoadTexture("c:/code/FPS/src/models/guitar/textures/1001_metallic.jpg");

    mesh->material.diffuse = albedo;
    mesh->material.specular = specular;

    return mesh;
}

GLuint LoadTexture(char* path_to_image);

Model* LoadModel(){
    Assimp::Importer importer;
    // const aiScene* scene = importer.ReadFile("c:/code/FPS/src/cube.fbx", aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    Model* model = (Model*)Malloc(sizeof(Model));
    const aiScene* scene = importer.ReadFile("c:/code/FPS/src/models/guitar/Survival_BackPack_2.fbx", aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        return NULL;
    }

    model->mesh_count = scene->mNumMeshes;
    model->meshes = (Mesh**)Malloc(sizeof(Mesh*) * model->mesh_count);

    for(int i = 0; i < scene->mRootNode->mNumChildren; i ++){
        u32 mesh_index = scene->mRootNode->mChildren[i]->mMeshes[0];
        mat4* transform = (mat4*)&(scene->mRootNode->mChildren[i]->mTransformation.a1);
        model->meshes[i] = LoadMesh(scene->mMeshes[mesh_index], *transform);

        aiMaterial* material = scene->mMaterials[scene->mMeshes[mesh_index]->mMaterialIndex];
        u32 count = material->GetTextureCount(aiTextureType_DIFFUSE);
        int y = 4;
    }

    return model;
}

void DrawMesh(Mesh* mesh){

    if(!mesh->shader){
        // ShaderCreate("c:/code/fps/src/shaders/test_cube_vertex.hlsl", "c:/code/fps/src/shaders/test_cube_fragment.hlsl", &mesh->shader);
        ShaderCreate("c:/code/fps/src/shaders/backpack_vertex.hlsl", "c:/code/fps/src/shaders/backpack_fragment.hlsl", &mesh->shader);
    }
    if(!mesh->ebo){
        glGenBuffers(1, &mesh->ebo);
    }
    if(!mesh->vao){
        glGenVertexArrays(1, &mesh->vao);
    }
    if(!mesh->vbo){
        glGenBuffers(1, &mesh->vbo);
    }

    glBindVertexArray(mesh->vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mesh->indices[0]) * mesh->index_count, mesh->indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mesh->vertices[0]) * mesh->vertex_count, mesh->vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    static r32 rot = 0;
    // rot++;

    mat4 scale = CreateScaleMatrix({0.01f, 0.01f, 0.01f});
    mat4 rotation = CreateRotationMatrix(Normalize({1.0f, 1.0f, 1.0f}), rot);
    mat4 translation = CreateTranslationMatrix({0, 1, 1.5f});
    mat4 model_mat = scale * rotation * translation;

    mat4 projection = CreatePerspectiveMatrix(ToRadians(90.0f), 0.01f, 1000.0f, 1280.0f, 720.0f);

    mat4 view = LookAt(game_state->camera_p, game_state->camera_dir + game_state->camera_p);

    ShaderSetUniform(mesh->shader, "model", model_mat);
    ShaderSetUniform(mesh->shader, "view", view);
    ShaderSetUniform(mesh->shader, "projection", projection);

    ShaderSetUniform(mesh->shader, "material.shininess", 32.0f);

    ShaderSetUniform(mesh->shader, "material.diffuse", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh->material.diffuse);

    ShaderSetUniform(mesh->shader, "material.specular", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, mesh->material.specular);


    Point_Light p0;
    p0.position = v3{0.0f, 0.0f, 3.0f};
    p0.light_color = v3{1.0f, 1.0f, 1.0f};
    p0.ambient = v3{0.2f, 0.2f, 0.2f};
    p0.diffuse = v3{0.4f, 0.4f, 0.4f};
    p0.specular = v3{1.0f, 1.0f, 1.0f};
    p0.constant = 1.0f;
    p0.linear = 0.07f;
    p0.quadratic = 0.017f;

    AddPointLight(mesh->shader, p0, 0);

    ShaderSetUniform(mesh->shader, "camera_pos", game_state->camera_p);
    ShaderSetUniform(mesh->shader, "camera_dir" ,game_state->camera_dir);

    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);


}

void TestAssimp(HWND window){

    static u32 init = 0;
    static Model* model;
    if(!init){
        model = LoadModel();
        init = 1;
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.3f, 0.2f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    for(int i = 0; i < model->mesh_count; i ++){
        DrawMesh(model->meshes[i]);
    }

    HDC device_context = GetDC(window);
    SwapBuffers(device_context);
    ReleaseDC(window, device_context);
}