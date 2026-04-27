@echo off
if not exist bin mkdir bin

echo [1/2] Trying FreeType-enabled build...
g++ -std=c++17 -Wall -DCRAM_ENABLE_FREETYPE=1 ^
    src\main.cpp src\scheduler.cpp src\imgui_freetype.cpp ^
    libs\imgui\imgui.cpp libs\imgui\imgui_draw.cpp libs\imgui\imgui_tables.cpp libs\imgui\imgui_widgets.cpp ^
    libs\imgui\imgui_impl_glfw.cpp libs\imgui\imgui_impl_opengl3.cpp ^
    -I src -I libs\imgui -I libs\glfw\include -I libs\freetype\include ^
    -L libs\glfw\lib -L libs\freetype ^
    -l glfw3 -l opengl32 -l gdi32 -l user32 -l freetype ^
    -o bin\cramtasker.exe

if %ERRORLEVEL% == 0 (
    echo Build OK (FreeType enabled) - run bin\cramtasker.exe
    goto :eof
)

echo [2/2] FreeType link failed, falling back to default ImGui font builder...
g++ -std=c++17 -Wall -DCRAM_ENABLE_FREETYPE=0 ^
    src\main.cpp src\scheduler.cpp ^
    libs\imgui\imgui.cpp libs\imgui\imgui_draw.cpp libs\imgui\imgui_tables.cpp libs\imgui\imgui_widgets.cpp ^
    libs\imgui\imgui_impl_glfw.cpp libs\imgui\imgui_impl_opengl3.cpp ^
    -I src -I libs\imgui -I libs\glfw\include ^
    -L libs\glfw\lib ^
    -l glfw3 -l opengl32 -l gdi32 -l user32 ^
    -o bin\cramtasker.exe

if %ERRORLEVEL% == 0 (
    echo Build OK (fallback mode) - run bin\cramtasker.exe
) else (
    echo Build FAILED
)
