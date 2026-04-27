#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <cstring>
#include "scheduler.h"

using namespace CramCore;

static void renderTopBanner(CramTasker& planner) {
    ImGui::TextUnformatted("Study planner dashboard");
    ImGui::TextDisabled("1. Add subjects. 2. Add tasks. 3. Use greedy or DP to compare schedules.");
    ImGui::Separator();

    ImGui::Text("Subjects: %zu", planner.subjectCount());
    ImGui::SameLine();
    ImGui::Text("Tasks: %zu", planner.taskCount());
    ImGui::SameLine();
    ImGui::TextDisabled("Tip: update grades directly in the subjects table.");
    ImGui::Spacing();
}

static void renderSubjectListPanel(CramTasker& planner) {
    ImGui::SeparatorText("Subjects (Editable Grades)");
    ImGui::TextDisabled("Drag the grade slider to update priorities instantly.");

    const auto& subs = planner.getSubjects();
    if (subs.empty()) {
        ImGui::TextWrapped("No subjects added yet. Start by creating a subject on the left, then add tasks for it.");
        return;
    }

    ImGui::BeginChild("subj_list", ImVec2(0, 150), true);
    if (ImGui::BeginTable("SubjTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Order", ImGuiTableColumnFlags_WidthFixed, 40.0f);
        ImGui::TableSetupColumn("Credits", ImGuiTableColumnFlags_WidthFixed, 45.0f);
        ImGui::TableSetupColumn("Points", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Prior.", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableHeadersRow();

        for (const auto& [key, s] : subs) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", s.name.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%d", s.examOrder);
            ImGui::TableNextColumn();
            ImGui::Text("%d", s.credits);
            ImGui::TableNextColumn();
            
            int pts = s.points;
            ImGui::PushID(key.c_str());
            ImGui::SetNextItemWidth(70.0f);
            if (ImGui::SliderInt("##gr", &pts, 0, 100)) {
                planner.updateGrade(key, pts);
            }
            ImGui::PopID();
            
            ImGui::TableNextColumn();
            ImGui::Text("%.0f", s.calcPriority());
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();
}

static void renderAddSubjectPanel(CramTasker& planner) {
    static char name[64]  = "";
    static int  points    = 70;
    static int  examOrder = 1;
    static int  credits   = 3;

    ImGui::SeparatorText("Add Subject");
    ImGui::TextDisabled("Create a course with its points, exam order, and credits.");
    ImGui::Dummy(ImVec2(0.0f, 2.0f));

    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);
    ImGui::InputTextWithHint("Name##subj", "e.g. Algorithms", name, sizeof(name));
    
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);
    ImGui::SliderInt("Points", &points, 0, 100);
    
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);
    ImGui::SliderInt("Exam Order", &examOrder, 1, 30);
    
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);
    ImGui::SliderInt("Credits", &credits, 1, 10);
    
    ImGui::Dummy(ImVec2(0.0f, 4.0f));

    if (ImGui::Button("Add Subject", ImVec2(140, 32))) {
        if (name[0] != '\0') {
            planner.addSubject(name, points, examOrder, credits);
            memset(name, 0, sizeof(name));
        }
    }
    ImGui::SameLine(0, 10.0f);
    if (ImGui::Button("Clear", ImVec2(100, 32))) {
        memset(name, 0, sizeof(name));
        points = 70;
        examOrder = 1;
        credits = 3;
    }
}

static void renderAddTaskPanel(CramTasker& planner) {
    static char title[128] = "";
    static char subKey[64] = "";
    static int  startH     = 9;
    static int  endH       = 10;
    static int  prio       = 5;

    ImGui::SeparatorText("Add Task");
    ImGui::TextDisabled("Choose a subject, then set the study window.");
    ImGui::InputTextWithHint("Title##task", "e.g. Revise sorting", title, sizeof(title));

    const auto& subjects = planner.getSubjects();
    if (subjects.empty()) {
        ImGui::BeginDisabled();
        if (ImGui::BeginCombo("Subject", "Add a subject first")) {
            ImGui::EndCombo();
        }
        ImGui::EndDisabled();
        ImGui::TextWrapped("You need at least one subject before creating a task.");
    } else {
        if (ImGui::BeginCombo("Subject", subKey[0] == '\0' ? "(select)" : subKey)) {
            for (const auto& [k, s] : subjects) {
                bool is_selected = (strcmp(subKey, k.c_str()) == 0);
                if (ImGui::Selectable(k.c_str(), is_selected)) {
                    memset(subKey, 0, sizeof(subKey));
                    strncpy(subKey, k.c_str(), sizeof(subKey) - 1);
                }
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    ImGui::SliderInt("Start hour",    &startH, 0, 23);
    ImGui::SliderInt("End hour",      &endH,   1, 24);
    ImGui::SliderInt("Priority",      &prio,   1, 10);
    ImGui::TextDisabled("Higher priority means the task appears more urgent in the heap view.");

    ImGui::BeginDisabled(subjects.empty());
    bool addPressed = ImGui::Button("Add Task");
    ImGui::EndDisabled();

    if (addPressed && !subjects.empty()) {
        if (title[0] != '\0' && subKey[0] != '\0' && endH > startH) {
            planner.addTask(title, subKey, startH, endH, prio);
            memset(title,  0, sizeof(title));
        }
    }
}

static void renderSchedulePanel(CramTasker& planner) {
    ImGui::SeparatorText("Schedule");
    ImGui::TextDisabled("Completed tasks are dimmed. Conflicts are highlighted in red.");

    if (ImGui::Button("Sort by End Time")) planner.sortByEnd();
    ImGui::SameLine();
    ImGui::TextDisabled("Tasks: %zu", planner.taskCount());

    ImGui::BeginChild("sched_list", ImVec2(0, 220), true);
    if (ImGui::BeginTable("SchedTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("Done", ImGuiTableColumnFlags_WidthFixed, 52.0f);
        ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableSetupColumn("Task");
        ImGui::TableSetupColumn("Weight", ImGuiTableColumnFlags_WidthFixed, 88.0f);
        ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 72.0f);
        ImGui::TableHeadersRow();

        const auto& tasks = planner.getTasks();
        int lastEnd = -1;
        
        std::string taskToDelete = "";

        for (size_t i = 0; i < tasks.size(); ++i) {
            const auto& t = tasks[i];
            ImGui::TableNextRow();
            
            // Checkmark
            ImGui::TableNextColumn();
            bool done = t.completed;
            ImGui::PushID(t.title.c_str());
            if (ImGui::Checkbox("##done", &done)) {
                planner.toggleTaskCompletion(t.title);
            }
            ImGui::PopID();

            // Time with conflict warning
            ImGui::TableNextColumn();
            bool conflict = (lastEnd != -1 && t.startHour < lastEnd);
            if (conflict) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
            ImGui::Text("%02d:00 - %02d:00", t.startHour, t.endHour);
            if (conflict) ImGui::PopStyleColor();
            if (!conflict || t.endHour > lastEnd) lastEnd = t.endHour;
            
            if (done) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
            
            // Title & Subject
            ImGui::TableNextColumn();
            ImGui::Text("%s", t.title.c_str());
            ImGui::TextDisabled("%s", t.subjectKey.c_str());

            // Weight
            ImGui::TableNextColumn();
            ImGui::Text("%.0f", t.weight);

            if (done) ImGui::PopStyleColor();

            // Action
            ImGui::TableNextColumn();
            ImGui::PushID(t.title.c_str());
            if (ImGui::Button("Delete")) {
                taskToDelete = t.title;
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
        
        if (!taskToDelete.empty()) {
            planner.removeTask(taskToDelete);
        }
    }
    ImGui::EndChild();
}

static void renderUrgentPanel(CramTasker& planner) {
    static int topN = 3;
    ImGui::SeparatorText("Urgent (Heap top-N)");
    ImGui::TextDisabled("Shows the most urgent tasks based on calculated weight.");
    ImGui::SliderInt("Top N", &topN, 1, 10);

    if (ImGui::Button("Show Urgent"))
        planner.displayUrgent(topN);

    ImGui::TextDisabled("(see terminal)");
}

static void renderGreedyPanel(CramTasker& planner) {
    static std::vector<Task> result;

    ImGui::SeparatorText("Greedy Activity Selection");
    ImGui::TextDisabled("Maximizes the number of non-overlapping study blocks.");
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

    ImGui::SeparatorText("DP Weighted Scheduling");
    ImGui::TextDisabled("Chooses the best total-priority schedule.");

    if (ImGui::Button("Run DP")) {
        dpResult = planner.dpSchedule();
    }

    if (dpResult.empty()) {
        ImGui::TextDisabled("Run to see optimal schedule.");
    } else {
        ImGui::BeginChild("dp_list", ImVec2(0, 150), true);
        for (const auto& t : dpResult) {
            ImGui::Text("%02d:00 - %02d:00  %s", t.startHour, t.endHour,
                        t.title.c_str());
        }
        ImGui::EndChild();
    }
}

static void applyModernStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Rounding
    style.WindowRounding    = 8.0f;
    style.FrameRounding     = 6.0f;
    style.PopupRounding     = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding      = 6.0f;
    style.TabRounding       = 6.0f;
    style.ChildRounding     = 6.0f;

    // Padding & Spacing
    style.WindowPadding     = ImVec2(20, 20);
    style.FramePadding      = ImVec2(14, 10);
    style.ItemSpacing       = ImVec2(14, 12);
    style.ItemInnerSpacing  = ImVec2(10, 10);

    // Colors (Modern Dark Theme)
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.54f, 0.58f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.12f, 0.13f, 0.16f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.15f, 0.16f, 0.20f, 1.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.11f, 0.12f, 0.15f, 1.00f);
    colors[ImGuiCol_Border]                 = ImVec4(0.25f, 0.26f, 0.30f, 1.00f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.28f, 0.30f, 0.36f, 1.00f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.35f, 0.40f, 0.55f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.12f, 0.13f, 0.16f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.15f, 0.16f, 0.20f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.12f, 0.13f, 0.16f, 1.00f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.12f, 0.13f, 0.16f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.10f, 0.11f, 0.13f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.25f, 0.26f, 0.30f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.35f, 0.40f, 0.55f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.45f, 0.50f, 0.65f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.45f, 0.55f, 0.85f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.45f, 0.55f, 0.85f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.55f, 0.65f, 0.95f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.25f, 0.35f, 0.65f, 1.00f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.35f, 0.45f, 0.75f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.45f, 0.55f, 0.85f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.25f, 0.35f, 0.65f, 1.00f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.35f, 0.45f, 0.75f, 1.00f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.45f, 0.55f, 0.85f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.25f, 0.26f, 0.30f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.35f, 0.40f, 0.55f, 1.00f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.45f, 0.50f, 0.65f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.25f, 0.35f, 0.65f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.35f, 0.45f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.45f, 0.55f, 0.85f, 1.00f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.18f, 0.20f, 0.25f, 1.00f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.35f, 0.45f, 0.75f, 1.00f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.25f, 0.35f, 0.65f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.15f, 0.16f, 0.20f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.18f, 0.20f, 0.25f, 1.00f);
}

static void setupFontsBuiltin() {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();

    ImFontConfig config;
    config.OversampleH = 3;
    config.OversampleV = 3;
    config.RasterizerMultiply = 1.0f;
    config.PixelSnapH = true;

    // Use a larger crisp font (26px) for great readability.
    ImFont* base = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/segoeui.ttf", 26.0f, &config, io.Fonts->GetGlyphRangesCyrillic());
    if (!base) {
        // Fallback to default
        base = io.Fonts->AddFontDefault();
    }
    io.FontDefault = base;
}

int main() {
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 820, "CramTasker", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    applyModernStyle();
    setupFontsBuiltin();

    // Increase general UI elements padding, dimensions and spacing for readability
    ImGui::GetStyle().ScaleAllSizes(1.35f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    CramTasker planner;

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

        renderTopBanner(planner);

        float leftW = ImGui::GetContentRegionAvail().x * 0.38f;
        if (leftW < 420.0f) leftW = 420.0f;
        ImGui::BeginChild("left", ImVec2(leftW, 0), false);
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