#include "game.h"
#include <windows.h>
#include "windowsx.h"
#include <xinput.h>
#include <dsound.h>
#include "Lib.h"

#define PI 3.14159265359f

HWND window_handle;

File ReadEntireFile(char* file){
    File game_file = {0};
    HANDLE file_handle = CreateFileA( file, GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD error_code = GetLastError();
    if(error_code == 0){
        LARGE_INTEGER file_size = {0};
        if(GetFileSizeEx( file_handle, &file_size)){
            game_file.size = file_size.QuadPart;
            game_file.memory = VirtualAlloc(0, game_file.size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
            if(ReadFile(file_handle, game_file.memory, game_file.size, 0, NULL)){
            }
        }
    }
    return game_file;
}

void FreeEntireFile(File game_file){
    VirtualFree(game_file.memory, 0, MEM_RELEASE);
}


typedef void type_RenderGame(HWND window_handle, IO* io, Memory memory, float frame_time, Game_Input game_input, RECT client_rect);
type_RenderGame* RenderGame;

struct offscreen_buffer{
    int WIDTH;
    int HEIGHT;
    BITMAPINFO bitmapinfo;
    void* bitmap_memory;
};

struct Program_Memory{
    void* memory;
};

global XINPUT_STATE controller_state;
global offscreen_buffer backbuffer;
global b32 running;
global r32 performance_counter_per_second;

global b32 recording;
global b32 replaying;

DWORD WINAPI XInputGetStateStub(DWORD dwUserIndex, XINPUT_STATE* pState){
    return 0;
}

static int sample_rate = 44100;
static int number_channels = 2;
static int bytes_per_sample = 2;
static int buffer_size_seconds = 1;
static LPDIRECTSOUNDBUFFER secondary_buffer = NULL;
// int* offscreen_buffer = NULL;

void InitDirectSound(HWND window_handle){

    HRESULT hr;

    int audio_buffer_size_bytes = sample_rate * number_channels * bytes_per_sample * buffer_size_seconds;

    WAVEFORMATEX wave_format = {0};
    wave_format.wFormatTag = WAVE_FORMAT_PCM;
    wave_format.nChannels = number_channels;
    wave_format.nSamplesPerSec = sample_rate;
    wave_format.nAvgBytesPerSec  = sample_rate * number_channels * bytes_per_sample;
    wave_format.nBlockAlign = number_channels * bytes_per_sample;
    wave_format.wBitsPerSample = bytes_per_sample * 8;
    wave_format.cbSize = NULL;

    byte* audio_buffer = (byte*)malloc(sizeof(byte) * audio_buffer_size_bytes);

    DSBUFFERDESC buffer_desc = {0};
    buffer_desc.dwSize          = sizeof(DSBUFFERDESC);
    buffer_desc.dwFlags         = DSBCAPS_GLOBALFOCUS;
    buffer_desc.dwBufferBytes   = audio_buffer_size_bytes;
    buffer_desc.dwReserved      = 0;
    buffer_desc.lpwfxFormat     = &wave_format;
    buffer_desc.guid3DAlgorithm = DS3DALG_DEFAULT;

    IDirectSound8* direct_sound = 0;

    hr = DirectSoundCreate8( NULL, &direct_sound, NULL);
    hr = direct_sound->SetCooperativeLevel( window_handle, DSSCL_PRIORITY);

    IDirectSoundBuffer* tmp_buffer = NULL;

    hr = direct_sound->CreateSoundBuffer( &buffer_desc, &tmp_buffer, NULL);
    hr = tmp_buffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&secondary_buffer);

    hr = secondary_buffer->SetFormat(&wave_format);

}

struct Wav_Header{
    char RIFF[4];
    int32_t file_size;
    char WAVE[4];
    char fmt[4];
    int32_t length_format_data;
    int16_t format_type;
    int16_t num_channels;
    int32_t sample_rate;
    int32_t bytes_per_second;
    int16_t something;
    int16_t bits_per_sample;
    char data[4];
    int32_t data_size;
};

void PlayWaveFile(File* f){

    void* block_1_pointer = NULL;
    DWORD block_1_bytes = 0;
    void* block_2_pointer = NULL;
    DWORD block_2_bytes = 0;


    static void* audio_buffer_base_address = NULL;
    if(audio_buffer_base_address == NULL){
        secondary_buffer->Lock(0, 4, &audio_buffer_base_address, &block_1_bytes, &block_2_pointer, &block_2_bytes, 0);
        secondary_buffer->Unlock(audio_buffer_base_address, block_1_bytes, block_2_pointer, block_2_bytes);
        block_1_bytes = 0;
        block_2_pointer = NULL;
        block_2_bytes = 0;
    }

    int audio_buffer_size_bytes = sample_rate * number_channels * bytes_per_sample * buffer_size_seconds;
    int total_size_sample = bytes_per_sample * number_channels;
    int samples_to_write = sample_rate/20;

    static int current_write_position = 0;

    HRESULT hr;

    DWORD play_cursor;
    DWORD write_cursor;
    hr = secondary_buffer->GetCurrentPosition( &play_cursor, &write_cursor);

    int num_samples_to_be_played = 0;
    if(current_write_position < play_cursor){
        num_samples_to_be_played = (audio_buffer_size_bytes - play_cursor) + current_write_position;
    }
    else{
        num_samples_to_be_played = current_write_position - play_cursor;
    }

    static int num_samples_played = 0;

    char* wav_data_start_byte = (char*)f->memory + sizeof(Wav_Header);

    if(num_samples_to_be_played < sample_rate/5){
        HRESULT hrr = secondary_buffer->Lock(current_write_position, samples_to_write * total_size_sample, &block_1_pointer, &block_1_bytes, &block_2_pointer, &block_2_bytes, 0);
        int num_samples_block1 = block_1_bytes / (bytes_per_sample * number_channels);
        for(DWORD i = 0; i < num_samples_block1; i ++ ){
            int16_t* cur_sample = &(((int16_t*)block_1_pointer)[i * 2]);

            int16_t* sample_left = cur_sample;
            int16_t* sample_right= cur_sample + 1;

            char* sample_data = wav_data_start_byte + (num_samples_played * 4);

            int16_t sample_left_value = *((int16_t*)sample_data);
            int16_t sample_right_value = *((int16_t*)(sample_data + 2));

            *sample_left = sample_left_value;
            *sample_right= sample_right_value;


            num_samples_played += 1;
        }
        current_write_position += num_samples_block1 * total_size_sample;

        int num_samples_block2 = block_2_bytes / (bytes_per_sample * number_channels);
        for(DWORD i = 0; i < num_samples_block2; i ++ ){
            int16_t* cur_sample = &(((int16_t*)block_1_pointer)[i * 2]);

            int16_t* sample_left = cur_sample;
            int16_t* sample_right= cur_sample + 1;

            char* sample_data = wav_data_start_byte + (num_samples_played * 4);

            int16_t sample_left_value = *((int16_t*)sample_data);
            int16_t sample_right_value = *((int16_t*)(sample_data + 2));

            num_samples_played += 1;
        }

        current_write_position += num_samples_block2 * total_size_sample;
        current_write_position %= audio_buffer_size_bytes;

        hr = secondary_buffer->Unlock( block_1_pointer, block_1_bytes, block_2_pointer, block_2_bytes);
    }
}



internal void ResizeDIBSection(int new_bitmap_width, int new_bitmap_height, offscreen_buffer* buffer){

        buffer->WIDTH = new_bitmap_width;
        buffer->HEIGHT = new_bitmap_height;

        if(buffer->bitmap_memory){
            VirtualFree(buffer->bitmap_memory, 0, MEM_RELEASE);
        }

        buffer->bitmapinfo = {};
        BITMAPINFOHEADER bitmapinfo_header = {};

        bitmapinfo_header.biSize = sizeof(buffer->bitmapinfo.bmiHeader);
        bitmapinfo_header.biWidth = buffer->WIDTH;

        //if biHeight is negative 0,0 is top left, if positive, 0,0 is bottom left
        bitmapinfo_header.biHeight = buffer->HEIGHT;
        bitmapinfo_header.biPlanes = 1;
        bitmapinfo_header.biBitCount = 32;
        bitmapinfo_header.biCompression = BI_RGB;

        buffer->bitmapinfo.bmiHeader = bitmapinfo_header;

        buffer->bitmap_memory = VirtualAlloc( 0, buffer->WIDTH*buffer->HEIGHT*4, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
}

WINDOWPLACEMENT g_wpPrev = { sizeof(g_wpPrev) };
void ToggleFullScreen(HWND hwnd, int x, int y, UINT keyFlags){
    DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);
    if (dwStyle & WS_OVERLAPPEDWINDOW) {
        MONITORINFO mi = { sizeof(mi) };
        if (GetWindowPlacement(hwnd, &g_wpPrev) && GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &mi)) {
        SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
        SetWindowPos(hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    } 
    else {
        SetWindowLong(hwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(hwnd, &g_wpPrev); SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    LRESULT Result = 0;
    switch(uMsg){
        case WM_CLOSE:{
            running = false;
        } break;


        case WM_INPUT:{
            RAWINPUT* pData = NULL;
            PUINT pcbSize = NULL;
            // UINT some = GetRawInputData(
            //     (HRAWINPUT)message.lParam,
            //     RID_HEADER,
            //     pData,
            //     pcbSize,
            //     sizeof(RAWINPUTHEADER)
            // );
            UINT some = GetRawInputBuffer(
                // pData,
                NULL,
                pcbSize,
                sizeof(RAWINPUTHEADER)
            );
            DWORD error_code = GetLastError();
            int x = 4;

        }break;

        case WM_ACTIVATE:{
            if(wParam == 0){
                HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
                SetCursor(cursor);
            //     SetWindowPos(hwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
            }
        }break;

        case WM_SETCURSOR:{
            if(LOWORD(lParam) == HTCLIENT){
                HWND window = GetActiveWindow();
                if(window == hwnd){
                    SetCursor(NULL);
                }
            }
        }break;

        case WM_KEYUP:
        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:{

            persist b32 ctrl_down = 0;
            persist b32 alt_down = 0;
            b32 key_was_down = (((lParam) >> 30) & 1);
            b32 key_is_down = !(((lParam) >> 31) & 1);

            if(wParam == VK_MENU){
                if(key_is_down){
                    alt_down = 1;
                }
                else{
                    alt_down = 0;
                }
            }
            if(wParam == VK_CONTROL || wParam == VK_LCONTROL || wParam == VK_RCONTROL){
                if(key_is_down){
                    ctrl_down = 1;
                }
                else{
                    ctrl_down = 0;
                }
            }
            if(wParam == VK_RETURN){
                if(alt_down && key_is_down){
                    ToggleFullScreen(hwnd, 0, 0, 0);
                }
            }
            if (wParam == VK_ESCAPE){
                u16 thingy = GetAsyncKeyState(VK_CONTROL);
                if(GetAsyncKeyState(VK_CONTROL) >> 15 == 0){
                    running = false;
                }
            }
            else if (wParam == 'J'){
                replaying = 0;
                recording = !recording;
            }
            else if (wParam == 'K'){
                recording = 0;
                replaying = !replaying;
            }
            else if(wParam == 'L'){
                recording = 0;
                replaying = 0;
            }
        }break;

        case WM_SIZE:{
            RECT ClientRect; 
            GetClientRect(hwnd, &ClientRect);
            int Width = ClientRect.right - ClientRect.left;
            int Height = ClientRect.bottom - ClientRect.top;
            ResizeDIBSection(Width, Height, &backbuffer);
        }break;

        case WM_PAINT:{
            PAINTSTRUCT paint;
            HDC device_context = BeginPaint(hwnd, &paint);
            EndPaint(hwnd, &paint);
        }break;

        // case WM_SETCURSOR:{
            // HCURSOR cursor = LoadCursor( 0, MAKEINTRESOURCE(32651));
            // SetCursor(cursor);
        // }break;

        default:{
            Result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        } break;
    }
    return Result;
}

void HandleGameInput(Input_Button_State* button_state, b32 is_down){
    if(is_down){
        if(!button_state->is_down){
            button_state->transition_count += 1;
            button_state->is_down = 1;
        }
    }
    else if(!is_down){
        if(button_state->is_down){
            button_state->transition_count += 1;
            button_state->is_down = 0;
        }
    }
}

void HandleGameInput(Input_Trigger_State* trigger_state, u8 value){
    if(value > trigger_state->value){
        trigger_state->value = value;
    }
}

void HandleGameInput(Input_Stick_State* stick_state, s16 x, s16 y){
    if(Absolute(x) > Absolute(stick_state->x)){
        stick_state->x = x;
    }
    if(Absolute(y) > Absolute(stick_state->y)){
        stick_state->y = y;
    }
}

void MessageHandler(MSG message, Game_Input* game_input, HWND window_handle){
    switch (message.message){
        case WM_MOUSEMOVE:{
            if(GetActiveWindow() != window_handle){
                return;
            }
            Mouse_State prev_mouse = game_input->mouse;
            POINT mouse_coords;
            GetCursorPos(&mouse_coords);

            RECT window_rect = {0};
            GetClientRect(window_handle, &window_rect);
            u32 mid_x = (window_rect.right+window_rect.left)/2;
            u32 mid_y = (window_rect.bottom + window_rect.top)/2;

            // mouse_coords.x = GET_X_LPARAM(message.lParam) + window_rect.left;
            // mouse_coords.y = GET_Y_LPARAM(message.lParam) + window_rect.top;


            game_input->mouse.x = mouse_coords.x;
            game_input->mouse.y = mouse_coords.y;
            if(mouse_coords.x == mid_x && mouse_coords.y == mid_y){
                return;
            }
            game_input->mouse_diff.x = mouse_coords.x - prev_mouse.x;
            game_input->mouse_diff.y = mouse_coords.y - prev_mouse.y;

            game_input->mouse_diff.x = mid_x - mouse_coords.x;
            game_input->mouse_diff.y = mid_y - mouse_coords.y;
            SetCursorPos(mid_x, mid_y);

        }break;

        case WM_KEYDOWN:
        case WM_KEYUP:{
            b32 key_was_down = (((message.lParam) >> 30) & 1);
            b32 key_is_down = !(((message.lParam) >> 31) & 1);

            if(message.wParam == 'W'){
                HandleGameInput(&game_input->move_forward, key_is_down);
            }
            if(message.wParam == 'S'){
                HandleGameInput(&game_input->move_back, key_is_down);
            }
            if(message.wParam == 'A'){
                HandleGameInput(&game_input->move_left, key_is_down);
            }
            if(message.wParam == 'D'){
                HandleGameInput(&game_input->move_right, key_is_down);
            }
            if(message.wParam == 'Q'){
                HandleGameInput(&game_input->move_down, key_is_down);
            }
            if(message.wParam == 'E'){
                HandleGameInput(&game_input->move_up, key_is_down);
            }

        }break;


        default:{

        }break;

    }

    TranslateMessage(&message);
    DispatchMessage(&message);
}

internal void LoadGameCode(){

    HANDLE file_handle = CreateFileA("c:/code/FPS/build/mygl.dll", GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);

    persist FILETIME prev_write_time = {0};
    FILETIME current_write_time = {0};
    GetFileTime(file_handle, 0, 0, &current_write_time);

    CloseHandle(file_handle);
    if(CompareFileTime(&prev_write_time, &current_write_time) == -1){
        prev_write_time = current_write_time;

        persist HMODULE GameLibrary = 0;
        if(GameLibrary){
            FreeLibrary(GameLibrary);
        }
        CopyFileA("c:/code/FPS/build/mygl.dll", "c:/code/FPS/build/tmpmygl.dll", 0);
        GameLibrary = LoadLibraryA("c:/code/FPS/build/tmpmygl.dll");
        RenderGame = (type_RenderGame*)GetProcAddress(GameLibrary, "RenderGame");
    }
}


struct Input_Queue{
    Game_Input value;
    r32 frame_delta;
    Input_Queue* next;
};

void QueuePush(Input_Queue** q, Game_Input value, r32 frame_delta){

    Input_Queue* new_node = (Input_Queue*)VirtualAlloc(0, sizeof(Input_Queue), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    new_node->next = NULL;
    new_node->value = value;
    new_node->frame_delta = frame_delta;

    Input_Queue* current = *q;
    if(current == NULL){
        *q = new_node;
        return;
    }
    while(current->next != NULL){
        current = current->next;
    }
    current->next = new_node;
}

void QueueDeleteAll(Input_Queue* q){
    if(q->next){
        QueueDeleteAll(q->next);
    }
    VirtualFree(q, 0, MEM_RELEASE);
    
}

struct InputRecording{
    void* game_memory_state;
    Input_Queue* input_queue;
    b32 first_pass;
    b32 init;
};

#include "stdio.h"

r32 CounterDifferenceInSeconds(LARGE_INTEGER a, LARGE_INTEGER b){
    return (a.QuadPart - b.QuadPart) / performance_counter_per_second;
}

int CALLBACK WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int iCmdShow) {

    IO io;
    io.FreeEntireFile = FreeEntireFile;
    io.ReadEntireFile = ReadEntireFile;

    LARGE_INTEGER seed = {};
    QueryPerformanceCounter(&seed);
    srand((uint32_t)seed.QuadPart);

    WNDCLASSA cls = {};
    cls.style = CS_HREDRAW|CS_VREDRAW;
    cls.lpfnWndProc = WindowProc;
    cls.hInstance = hThisInstance;
    cls.lpszClassName = "heya";

    InputRecording input_recording = {0};
    input_recording.init = 0;

    Game_Buffer game_buffer;
    Game_Input game_input = {0};

    LARGE_INTEGER performance_frequency;
    QueryPerformanceFrequency(&performance_frequency);
    performance_counter_per_second = (r32)performance_frequency.QuadPart;
    LARGE_INTEGER start_counter = {0};
    LARGE_INTEGER counter_previous = {0};

    LARGE_INTEGER end_counter = {0};

    r32 target_frame_rate = 60;
    r32 target_frame_time = 1.0f/target_frame_rate;
    r32 frame_time = 1/target_frame_rate;
    u64 frame_counter = 0;

    Memory memory;
    memory.memory = VirtualAlloc((void*)(TERABYTES(4)), GIGABYTES(1), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
    memory.size = GIGABYTES(1);

    timeBeginPeriod(1);

    if(RegisterClassA(&cls)){
        window_handle = CreateWindowExA(
            0,
            cls.lpszClassName,
            "nothing",
            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            1280 + 16,
            720 + 39,
            0,
            0,
            hThisInstance,
            0
        );

        InitDirectSound(window_handle);

        // InitOpenGL(WindowHandle);

        RAWINPUTDEVICE raw_input_device[1];
        raw_input_device[0].usUsagePage = 0x01;
        raw_input_device[0].usUsage = 0x06;
        raw_input_device[0].dwFlags = 0;
        raw_input_device[0].hwndTarget = window_handle;
        BOOL result = RegisterRawInputDevices(raw_input_device, 1, sizeof(RAWINPUTDEVICE));
        // InitMemoryArena(GIGABYTES(1));
        
        File f = ReadEntireFile("C:/code/FPS/src/out.wav");

        if (window_handle){
            running = true;
            QueryPerformanceCounter(&end_counter);
            while(running){
                PlayWaveFile(&f);
//                secondary_buffer->Play(0, 0, DSBPLAY_LOOPING);
                game_input.mouse_diff = {0};

                LoadGameCode();

                MSG message;
                while(PeekMessage(&message, 0, 0, 0, PM_REMOVE)){
                    if (message.message == WM_QUIT){
                        running = false;
                    }
                    else{
                        MessageHandler(message, &game_input, window_handle);
                    }
                }
                // GetControllerInput(0, &game_input);

                // game_buffer.height = backbuffer.HEIGHT;
                // game_buffer.width = backbuffer.WIDTH;
                // game_buffer.memory = backbuffer.bitmap_memory;

                RECT window_rect = {0};

                // GetWindowRect(window_handle, &window_rect);

                POINT mouse_coords = {0};   
                GetCursorPos(&mouse_coords);

                if(recording){
                    if(!input_recording.init){
                        if(input_recording.input_queue){
                            QueueDeleteAll(input_recording.input_queue);
                            input_recording.input_queue = NULL;
                        }
                        input_recording.game_memory_state = VirtualAlloc(0, memory.size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
                        memcpy(input_recording.game_memory_state, memory.memory, memory.size);
                        input_recording.init = 1;
                    }
                    QueuePush(&input_recording.input_queue, game_input, frame_time);
                }

                else if(!recording){
                    input_recording.init = 0;
                }

                if(replaying){
                    if(input_recording.input_queue != NULL){
                        persist Input_Queue* current = input_recording.input_queue;
                        if(!input_recording.first_pass){
                            input_recording.first_pass = true;
                            memcpy(memory.memory, input_recording.game_memory_state, memory.size);
                        }
                        if(current == NULL){
                            current = input_recording.input_queue;
                            memcpy(memory.memory, input_recording.game_memory_state, memory.size);
                        }
                        game_input = current->value;
                        frame_time = current->frame_delta;
                        current = current->next;
                    }
                }

                if(frame_counter % (u32)target_frame_rate == 0){
                    char buf[255];
                    sprintf(buf, "Frame time %.1f -- FPS %.1f\n", frame_time*1000, 1.0f/frame_time);
                    SetWindowTextA(window_handle, buf);
                    OutputDebugStringA(buf);
                }

                frame_counter++;

                HDC device_context = GetDC(window_handle);
                RECT client_rect;
                GetClientRect( window_handle, &client_rect);

                RenderGame(window_handle, &io, memory, frame_time, game_input, client_rect);
                ReleaseDC(window_handle, device_context);

                QueryPerformanceCounter(&start_counter);
                frame_time = CounterDifferenceInSeconds(start_counter, end_counter);

                while(target_frame_time - frame_time > 0){
                    if((u32)(((target_frame_time - frame_time)*1000) >= 1)){
                        Sleep((u32)((target_frame_time - frame_time)*1000) - 1);
                    }
                    QueryPerformanceCounter(&start_counter);
                    frame_time = CounterDifferenceInSeconds(start_counter, end_counter);
                }

                QueryPerformanceCounter(&end_counter);
            }
        }
    }
}
