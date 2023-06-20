@echo off

set CommonCompilerFlags=-DDEBUG=1 -Od -W2 -FS -FC -Zi -nologo

IF "%myINIT%" == "" call vcvarsall.bat x64 && set myINIT=1

pushd ..\build

del *pdb

set includes=/I "c:\code\lib**" /I "c:\code\FPS\src\include**" /I "c:\code\FPS\src**" /I "c:\code\FPS\src\include\imgui" /I "c:\code\FPS\src\include\imgui\backends" 

@REM cl %CommonCompilerFlags% %includes% ../src/mygl.cpp ../src/include/imgui/imgui*.cpp -LD -link opengl32.lib Gdi32.lib user32.lib assimp-vc143-mtd.lib /PROFILE -incremental:no -PDB:mygl_%random%.pdb -EXPORT:RenderGame
cl %CommonCompilerFlags% %includes% ../src/mygl.cpp  -LD -link opengl32.lib Gdi32.lib user32.lib assimp-vc143-mtd.lib /PROFILE -incremental:no -PDB:mygl_%random%.pdb -EXPORT:RenderGame
cl %CommonCompilerFlags% %includes% ../src/win32_platform.cpp -link /PROFILE User32.lib Gdi32.lib Dsound.lib Winmm.lib dxguid.lib

popd
