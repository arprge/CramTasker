@echo off
if not exist bin mkdir bin

g++ -std=c++17 -Wall ^
    src\main.cpp src\scheduler.cpp ^
    libs\imgui\imgui.cpp ^
    libs\imgui\imgui_draw.cpp ^
    libs\imgui\imgui_tables.cpp ^
    libs\imgui\imgui_widgets.cpp ^
    libs\imgui\imgui_impl_glfw.cpp ^
    libs\imgui\imgui_impl_opengl3.cpp ^
    -I src ^
    -I libs\imgui ^
    -I libs\glfw\include ^
    -L libs\glfw\lib ^
    -l glfw3 -l opengl32 -l gdi32 -l user32 ^
    -o bin\cramtasker.exe

if %ERRORLEVEL% == 0 (
    echo Build OK — run bin\cramtasker.exe
) else (
    echo Build FAILED
)
