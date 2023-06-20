#include "game.h"
// #include "windows.h"
#include "DataStructures.h"

void FileHasChanged(char* file){
}

File ReadEntireFile(char* file){

    Hash_Table t;
    t.capacity = 5;
    t.count = 0;
    t.keys = (Text*)Malloc(sizeof(Text) * t.capacity);
    t.values = (void**)Malloc(sizeof(void*) * t.capacity);

    Text* tt = TextCreate("hello");

    t.Add(*TextCreate("hello1"), (void*)1);
    t.Add(*TextCreate("hello2"), (void*)2);
    t.Add(*TextCreate("hello3"), (void*)3);
    // t.Add(*TextCreate("hello4"), (void*)4);
    // t.Add(*TextCreate("hello5"), (void*)5);
    // t.Add(*TextCreate("hello6"), (void*)6);

    File game_file = {0};
    // HANDLE file_handle = CreateFileA( file, GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    // DWORD error_code = GetLastError();
    // if(error_code == 0){
    //     LARGE_INTEGER file_size = {0};
    //     if(GetFileSizeEx( file_handle, &file_size)){
    //         game_file.size = file_size.QuadPart;
    //         // game_file.memory = VirtualAlloc(0, game_file.size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    //         game_file.memory = (char*)Malloc(sizeof(char) * game_file.size);
    //         if(ReadFile(file_handle, game_file.memory, game_file.size, 0, NULL)){
    //         }
    //     }
    // }
    return game_file;
}

void FreeEntireFile(File game_file){
    // VirtualFree(game_file.memory, 0, MEM_RELEASE);
    Free(game_file.memory);
}