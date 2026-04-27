# CramTasker

CramTasker is a C++ based Personal Exam Prep Tasker and Scheduler. It is designed to optimize study time based on exam deadlines, subject difficulty, and course credits. The project features a graphical user interface built with [Dear ImGui](libs/imgui).

I created this project during my freshman year because I always relied on ToDo lists, but scheduling and deciding the order was too tedious. So, I decided to build a tool to automate that process.

## Features 

The project is structured to progressively implement advanced data structures and algorithms:

*   **Core Data Structures:**
    *   `std::vector` for task management ([src/task.h](src/task.h)).
    *   `std::unordered_map` for subject tracking and $O(1)$ lookups.
    *   `std::priority_queue` (Max-Heap) for surfacing urgent tasks based on calculated weights.
*   **Algorithms:**
    *   **Sorting:** $O(n \log n)$ sorting by end time.
    *   **Greedy Activity Selection:** $O(n \log n)$ scheduling to maximize the number of non-overlapping study sessions.
    *   **Dynamic Programming (Planned - Week 11):** $O(n \log n)$ Weighted Interval Scheduling to maximize total study "value" or weight.
    *   **Graphs (Planned - Week 8-10):** Free slot graph representation.

## Project Structure

*   [src/](src/) - Core application source code.
    *   [main.cpp](src/main.cpp) - ImGui UI implementation and application loop.
    *   [scheduler.h](src/scheduler.h) / [scheduler.cpp](src/scheduler.cpp) - `CramTasker` class handling the core logic and algorithms.
    *   [task.h](src/task.h) - Data models (`Task`, `Subject`) and comparators.
*   [libs/](libs/) - Third-party dependencies.
    *   `imgui/` - Dear ImGui for the graphical interface.
    *   `glfw/` - Windowing and input handling.
*   [build.bat](build.bat) - Windows build script.

## Build Instructions

This project is built for Windows using `g++` (C++17). Ensure you have MinGW or a compatible GCC toolchain installed and added to your system's PATH.

1. Open a terminal in the project root.
2. Run the build script:
   ```cmd
   build.bat


   
