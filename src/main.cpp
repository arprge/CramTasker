#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "scheduler.h"

using namespace CramCore;

static void renderTopBanner(CramTasker& planner) {
    ImGui::TextUnformatted("Study planner dashboard");
    ImGui::TextDisabled("1. Add subjects. 2. Automatically get an optimized study schedule.");
    ImGui::Separator();

    ImGui::Text("Subjects: %zu", planner.subjectCount());
    ImGui::SameLine();
    ImGui::TextDisabled("Tip: update grades directly in the subjects table.");
    ImGui::Spacing();
}

static void renderSubjectListPanel(CramTasker& planner) {
    ImGui::SeparatorText("Subjects (Editable Grades)");
    ImGui::TextDisabled("Enter the grade to update priorities instantly.");

    const auto& subs = planner.getSubjects();
    if (subs.empty()) {
        ImGui::TextWrapped("No subjects added yet. by creating a subject below.");
        return;
    }

    std::string subjectToDelete = "";

    ImGui::BeginChild("subj_list", ImVec2(0, 250), true);
    if (ImGui::BeginTable("SubjTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Credits", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Points", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Prior.", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        Date today; today.year = 2026; today.month = 4; today.day = 28;
        for (const auto& [key, s] : subs) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", s.name.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%d-%02d-%02d", s.examDate.year, s.examDate.month, s.examDate.day);
            ImGui::TableNextColumn();
            ImGui::Text("%d", s.credits);
            ImGui::TableNextColumn();
            
            int pts = s.points;
            ImGui::PushID(key.c_str());
            ImGui::SetNextItemWidth(110.0f);
            if (ImGui::InputInt("##gr", &pts, 0)) {
                pts = std::clamp(pts, 0, 60);
                planner.updateGrade(key, pts);
            }
            ImGui::PopID();
            
            ImGui::TableNextColumn();
            ImGui::Text("%.1f", s.calcPriority(today));

            ImGui::TableNextColumn();
            ImGui::PushID((key + "_del").c_str());
            if (ImGui::Button("Del")) {
                subjectToDelete = key;
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();

    // Удаляем предмет после завершения отрисовки таблицы, чтобы не сломать итератор
    if (!subjectToDelete.empty()) {
        planner.removeSubject(subjectToDelete);
    }
}

static void renderAddSubjectPanel(CramTasker& planner) {
    static char name[64]  = "";
    static int  points    = 30;
    static int  examMonth = 5;
    static int  examDay   = 1;
    static int  credits   = 3;

    const char* months[] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

    ImGui::SeparatorText("Add Subject");
    ImGui::TextDisabled("Create a course with its points, exam date, and credits.");
    ImGui::Dummy(ImVec2(0.0f, 2.0f));

    if (ImGui::BeginTable("AddSubjectTable", 2, ImGuiTableFlags_SizingFixedFit)) {
        ImGui::TableSetupColumn("Labels", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Inputs", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Name");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-1);
        ImGui::InputTextWithHint("##subj", "e.g. Algorithms", name, sizeof(name));

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Points");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputInt("##points", &points, 1, 5)) points = std::clamp(points, 0, 60);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Exam Date");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(180.0f);
        if (ImGui::BeginCombo("##month", months[examMonth - 1])) {
            for (int i = 0; i < 12; i++) {
                bool is_selected = (examMonth == i + 1);
                if (ImGui::Selectable(months[i], is_selected)) examMonth = i + 1;
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Day");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputInt("##day", &examDay, 1, 5)) examDay = std::clamp(examDay, 1, 31);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Credits");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputInt("##credits", &credits, 1, 1)) credits = std::clamp(credits, 1, 6);

        ImGui::EndTable();
    }
    
    ImGui::Dummy(ImVec2(0.0f, 4.0f));

    if (ImGui::Button("Add Subject", ImVec2(180, 0))) {
        if (name[0] != '\0') {
            Date d; d.year = 2026; d.month = examMonth; d.day = examDay;
            planner.addSubject(name, points, d, credits);
            memset(name, 0, sizeof(name));
        }
    }
    ImGui::SameLine(0, 10.0f);
    if (ImGui::Button("Clear", ImVec2(120, 0))) {
        memset(name, 0, sizeof(name));
        points = 30;
        examMonth = 5; examDay = 1;
        credits = 3;
    }
}

static void renderSchedulePanel(CramTasker& planner) {
    ImGui::SeparatorText("Suggested Study Schedule (Calendar View)");
    ImGui::TextDisabled("Based on formula: (credits * (60 - points)) / daysLeft");

    Date today; today.year = 2026; today.month = 4; today.day = 28;
    auto schedule = planner.generateSchedule(today);
    if (schedule.empty()) {
        ImGui::TextWrapped("No subjects to schedule.");
        return;
    }

    ImGui::BeginChild("sched_cal", ImVec2(0, 0), true);
    
    // Group schedule by month to display multiple calendars if needed
    int currentMonthDisplay = -1;
    
    for (size_t i = 0; i < schedule.size(); ) {
        int monthLabel = schedule[i].date.month;
        if (monthLabel != currentMonthDisplay) {
            if (currentMonthDisplay != -1) ImGui::Spacing();
            const char* months[] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
            int monthIdx = std::clamp(monthLabel - 1, 0, 11);
            ImGui::Text("%s %d", months[monthIdx], schedule[i].date.year);
            currentMonthDisplay = monthLabel;
            
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8, 8));
            if (ImGui::BeginTable(("CalTab_" + std::to_string(monthLabel)).c_str(), 7, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp)) {
                ImGui::TableSetupColumn("Mon"); ImGui::TableSetupColumn("Tue"); ImGui::TableSetupColumn("Wed");
                ImGui::TableSetupColumn("Thu"); ImGui::TableSetupColumn("Fri"); ImGui::TableSetupColumn("Sat"); ImGui::TableSetupColumn("Sun");
                ImGui::TableHeadersRow();
                
                // Keep track of day of week to format calendar properly
                int dayOfWeek = (schedule[i].date.day) % 7; 
                
                ImGui::TableNextRow(ImGuiTableRowFlags_None, 110.0f);
                for (int skip = 0; skip < dayOfWeek; skip++) {
                    ImGui::TableNextColumn();
                }

                while (i < schedule.size() && schedule[i].date.month == monthLabel) {
                    if (dayOfWeek >= 7) {
                        dayOfWeek = 0;
                        ImGui::TableNextRow(ImGuiTableRowFlags_None, 110.0f);
                    }
                    ImGui::TableNextColumn();
                    const auto& s = schedule[i];
                    
                    ImGui::TextDisabled("%d", s.date.day);
                    ImGui::Separator();
                    if (!s.subjectName.empty()) {
                        ImGui::TextWrapped("%s", s.subjectName.c_str());
                        ImGui::TextColored(ImVec4(0.8f, 0.4f, 0.4f, 1.0f), "Pr: %.1f", s.priority);
                    }
                    
                    dayOfWeek++;
                    i++;
                }

                ImGui::EndTable();
            }
            ImGui::PopStyleVar();
        }
    }
    
    ImGui::EndChild();
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

static void saveSubjects(CramTasker& planner, const std::string& filepath) {
    std::ofstream out(filepath);
    if (!out.is_open()) return;

    for (const auto& [key, s] : planner.getSubjects()) {
        out << s.name << "|" 
            << s.points << "|" 
            << s.examDate.year << "|" 
            << s.examDate.month << "|" 
            << s.examDate.day << "|" 
            << s.credits << "\n";
    }
}

static void loadSubjects(CramTasker& planner, const std::string& filepath) {
    std::ifstream in(filepath);
    if (!in.is_open()) return;

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string name, token;
        
        try {
            if (!std::getline(ss, name, '|')) continue;
            if (!std::getline(ss, token, '|')) continue;
            int points = std::stoi(token);
            if (!std::getline(ss, token, '|')) continue;
            int year = std::stoi(token);
            if (!std::getline(ss, token, '|')) continue;
            int month = std::stoi(token);
            if (!std::getline(ss, token, '|')) continue;
            int day = std::stoi(token);
            if (!std::getline(ss, token, '|')) continue;
            int credits = std::stoi(token);

            Date d; d.year = year; d.month = month; d.day = day;
            planner.addSubject(name, points, d, credits);
        } catch (...) {
            // Игнорируем некорректные строки в случае повреждения файла
        }
    }
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
    loadSubjects(planner, "subjects_data.txt");

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

        float leftW = ImGui::GetContentRegionAvail().x * 0.45f;
        if (leftW < 600.0f) leftW = 600.0f;
        ImGui::BeginChild("left", ImVec2(leftW, 0), false);
        renderSubjectListPanel(planner);
        ImGui::Spacing();
        renderAddSubjectPanel(planner);
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("right", ImVec2(0, 0), false);
        renderSchedulePanel(planner);
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

    saveSubjects(planner, "subjects_data.txt");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}