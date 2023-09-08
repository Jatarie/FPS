@echo off

set CommonCompilerFlags=-DDEBUG=1 -O2 -W2 -FS -FC -Zi -nologo

REM IF "%myINIT%" == "" call vcvarsall.bat x64 && set myINIT=1

cd c:/code/fps/build

del *pdb

REM set includes=/I "c:\code\lib**" /I "c:\code\FPS\src\include**" /I "c:\code\FPS\src**" /I "c:\code\FPS\src\include\imgui" /I "c:\code\FPS\src\include\imgui\backends" 
set includes=/I "c:\code\lib**" /I "c:\code\FPS\src\include**" /I "c:\code\FPS\src**"  /I "c:\code\fps\src\include\glm" /I "c:\code\fps\src\include\glm\gtc"
set includes_win=/I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\ucrt" /I "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.35.32215\include" /I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\um" /I "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22000.0\shared"  /I "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.35.32215\include" /I"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.36.32532\include"


set LIBS=/LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22000.0\um\x64" /LIBPATH:"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.36.32532\lib\x64" /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22000.0\ucrt\x64" /LIBPATH:"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.35.32215\lib\x64\"

cl %CommonCompilerFlags% %includes% %includes_win% ../src/mygl.cpp -LD -link %LIBS% opengl32.lib Gdi32.lib user32.lib  /PROFILE -incremental:no -PDB:mygl_%random%.pdb -EXPORT:RenderGame
REM assimp-vc143-mtd.lib

REM cl %CommonCompilerFlags% %includes% ../src/mygl.cpp  -LD -link opengl32.lib Gdi32.lib user32.lib assimp-vc143-mtd.lib /PROFILE -incremental:no -PDB:mygl_%random%.pdb -EXPORT:RenderGame

REM %CommonCompilerFlags% %includes% c:/code/fps/src/mygl.cpp  -LD -link opengl32.lib Gdi32.lib user32.lib /PROFILE -incremental:no -PDB:mygl_%random%.pdb -EXPORT:RenderGame
cl %CommonCompilerFlags% %includes% %includes_win% ../src/win32_platform.cpp -link /PROFILE %LIBS% User32.lib Gdi32.lib Dsound.lib Winmm.lib dxguid.lib

cd c:/code/fps/src
