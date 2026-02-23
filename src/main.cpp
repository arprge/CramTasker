#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <cstring>
#include "scheduler.h"

using namespace CramCore;

static void loadTestData(CramTasker& planner) {
    planner.addSubject("ASD",    5, 62);
    planner.addSubject("LinAlg", 4, 78);
    planner.addSubject("OS",     3, 55);

    planner.addTask("Review heaps",     "ASD",    9, 11, 7);
    planner.addTask("DP exercises",     "ASD",   13, 15, 8);
    planner.addTask("Matrix mult",      "LinAlg",10, 12, 5);
    planner.addTask("Scheduling lab",   "OS",    14, 16, 6);
    planner.addTask("Graph traversal",  "ASD",   16, 18, 9);
}

static void renderSubjectListPanel(CramTasker& planner) {
    ImGui::SeparatorText("Subjects");

    const auto& subs = planner.getSubjects();
    if (subs.empty()) {
        ImGui::TextDisabled("No subjects added yet");
        return;
    }

    ImGui::BeginChild("subj_list", ImVec2(0, 100), true);
    for (const auto& [key, s] : subs) {
        ImGui::Text("%-10s  cr=%d  gr=%d  prio=%.0f",
                    s.name.c_str(), s.credits, s.currentGrade,
                    s.calcPriority());
    }
    ImGui::EndChild();
}

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

    if (ImGui::Button("Sort##sched")) planner.sortByEnd();

    ImGui::SameLine();
    ImGui::TextDisabled("(%zu tasks, %zu subjects)",
                        planner.taskCount(), planner.subjectCount());

    ImGui::BeginChild("sched_list", ImVec2(0, 180), true);
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

    if (ImGui::Button("Show Urgent"))
        planner.displayUrgent(topN);

    ImGui::TextDisabled("(see terminal)");
}

static void renderGreedyPanel(CramTasker& planner) {
    static std::vector<Task> result;

    ImGui::SeparatorText("Greedy Activity Selection");
    if (ImGui::Button("Run Greedy"))
        result = planner.greedySchedule();

    if (!result.empty()) {
        ImGui::SameLine();
        ImGui::Text("=> %zu selected", result.size());
    }

    ImGui::BeginChild("greedy_list", ImVec2(0, 130), true);
    for (const auto& t : result) {
        ImGui::Text("%02d:00-%02d:00  %s  (w=%.0f)",
                    t.startHour, t.endHour, t.title.c_str(), t.weight);
    }
    ImGui::EndChild();
}

static void renderDPPanel(CramTasker& planner) {
    static std::vector<Task> dpResult;

    ImGui::SeparatorText("DP Weighted Scheduling [WIP]");

    if (ImGui::Button("Run DP")) {
        dpResult = planner.dpSchedule();
    }

    if (dpResult.empty()) {
        ImGui::TextColored(ImVec4(1, 0.8f, 0.3f, 1), "see terminal");
    } else {
        ImGui::BeginChild("dp_list", ImVec2(0, 100), true);
        for (const auto& t : dpResult) {
            ImGui::Text("%02d:00-%02d:00  %s", t.startHour, t.endHour,
                        t.title.c_str());
        }
        ImGui::EndChild();
    }
}

int main() {
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(960, 640, "CramTasker [WIP]", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    CramTasker planner;

    static bool seedLoaded = false;
    if (!seedLoaded) {
        loadTestData(planner);
        seedLoaded = true;
    }

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("CramTasker", nullptr,
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoResize   |
                     ImGuiWindowFlags_NoMove);

        ImGui::BeginChild("left", ImVec2(320, 0), false);
        renderSubjectListPanel(planner);
        ImGui::Spacing();
        renderAddSubjectPanel(planner);
        ImGui::Spacing();
        renderAddTaskPanel(planner);
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("right", ImVec2(0, 0), false);
        renderSchedulePanel(planner);
        ImGui::Spacing();
        renderUrgentPanel(planner);
        ImGui::Spacing();
        renderGreedyPanel(planner);
        ImGui::Spacing();
        renderDPPanel(planner);
        ImGui::EndChild();

        ImGui::End();

        ImGui::Render();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
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