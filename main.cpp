// =============================================================
//  CramTasker — GUI entry point (Dear ImGui + GLFW + OpenGL3)
//
//  SETUP (one-time):
//  1. Download ImGui: https://github.com/ocornut/imgui
//     Copy into libs/imgui/:
//       imgui.h / .cpp, imgui_draw.cpp, imgui_tables.cpp,
//       imgui_widgets.cpp, imgui_internal.h,
//       imgui_impl_glfw.h / .cpp, imgui_impl_opengl3.h / .cpp,
//       imgui_impl_opengl3_loader.h,
//       imstb_rectpack.h, imstb_textedit.h, imstb_truetype.h
//
//  2. Download GLFW prebuilt (MinGW 64-bit):
//     https://www.glfw.org/download.html
//     libs/glfw/include/GLFW/glfw3.h
//     libs/glfw/lib/libglfw3.a
//
//  3. Run build.bat
// =============================================================

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <cstring>
#include "scheduler.h"

using namespace CramCore;

// ---- helpers ------------------------------------------------
static void renderAddSubjectPanel(CramTasker& planner) {
    static char name[64]  = "";
    static int  credits   = 3;
    static int  grade     = 70;

    ImGui::SeparatorText("Add Subject");
    ImGui::InputText("Name##subj",  name, sizeof(name));
    ImGui::SliderInt("Credits",     &credits, 1, 10);
    ImGui::SliderInt("Grade (0-100)", &grade, 0, 100);

    if (ImGui::Button("Add Subject")) {
        if (name[0] != '\0') {
            planner.addSubject(name, credits, grade);
            memset(name, 0, sizeof(name));
        }
    }
}

static void renderAddTaskPanel(CramTasker& planner) {
    static char title[128] = "";
    static char subKey[64] = "";
    static int  startH     = 9;
    static int  endH       = 10;
    static int  prio       = 5;

    ImGui::SeparatorText("Add Task");
    ImGui::InputText("Title##task",   title,  sizeof(title));
    ImGui::InputText("Subject##task", subKey, sizeof(subKey));
    ImGui::SliderInt("Start hour",    &startH, 0, 23);
    ImGui::SliderInt("End hour",      &endH,   1, 24);
    ImGui::SliderInt("Priority",      &prio,   1, 10);

    if (ImGui::Button("Add Task")) {
        if (title[0] != '\0' && subKey[0] != '\0' && endH > startH) {
            planner.addTask(title, subKey, startH, endH, prio);
            memset(title,  0, sizeof(title));
            memset(subKey, 0, sizeof(subKey));
        }
    }
}

static void renderSchedulePanel(CramTasker& planner) {
    ImGui::SeparatorText("Schedule (sorted by end time)");

    if (ImGui::Button("Refresh")) planner.sortByEnd();

    ImGui::BeginChild("sched_list", ImVec2(0, 200), true);
    for (const auto& t : planner.getTasks()) {
        ImGui::Text("%02d:00-%02d:00  %-20s  [%s]  w=%.0f",
                    t.startHour, t.endHour,
                    t.title.c_str(), t.subjectKey.c_str(), t.weight);
    }
    ImGui::EndChild();
}

static void renderUrgentPanel(CramTasker& planner) {
    static int topN = 3;
    ImGui::SeparatorText("Urgent (Heap top-N)");
    ImGui::SliderInt("Top N", &topN, 1, 10);

    // build a local copy so displayUrgent can pop without touching planner
    // TODO: expose a getTopN() method from CramTasker for cleaner access
    if (ImGui::Button("Show Urgent"))
        planner.displayUrgent(topN);   // prints to stdout for now

    ImGui::TextDisabled("(see terminal output)");
}

static void renderGreedyPanel(CramTasker& planner) {
    static std::vector<Task> result;

    ImGui::SeparatorText("Greedy Activity Selection");
    if (ImGui::Button("Run Greedy"))
        result = planner.greedySchedule();

    ImGui::BeginChild("greedy_list", ImVec2(0, 150), true);
    for (const auto& t : result) {
        ImGui::Text("%02d:00-%02d:00  %s",
                    t.startHour, t.endHour, t.title.c_str());
    }
    ImGui::EndChild();
}

// ---- main ---------------------------------------------------
int main() {
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(900, 600, "CramTasker", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    CramTasker planner;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // full-screen dockable window
        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("CramTasker", nullptr,
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoResize   |
                     ImGuiWindowFlags_NoMove);

        // left column — inputs
        ImGui::BeginChild("left", ImVec2(300, 0), false);
        renderAddSubjectPanel(planner);
        ImGui::Spacing();
        renderAddTaskPanel(planner);
        ImGui::EndChild();

        ImGui::SameLine();

        // right column — results
        ImGui::BeginChild("right", ImVec2(0, 0), false);
        renderSchedulePanel(planner);
        ImGui::Spacing();
        renderUrgentPanel(planner);
        ImGui::Spacing();
        renderGreedyPanel(planner);
        ImGui::EndChild();

        ImGui::End();

        // render
        ImGui::Render();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}