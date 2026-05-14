#pragma once

#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include "imgui/imgui.h"

// ============================================================================
// DATA STRUCTURES
// ============================================================================

struct CalendarMember
{
    std::string name;
    std::string avatarColor; // hex color for avatar
};

struct CalendarTask
{
    int id;
    std::string title;
    std::string notes;
    int dayOfWeek;        // 0 = Monday, 6 = Sunday
    int startHour;        // 9-20 (9 AM - 8 PM)
    int startMinute;      // 0-59
    int endHour;
    int endMinute;
    std::string colorTag; // Color category (e.g., "Work", "Personal", "Urgent")
    ImVec4 color;         // RGBA color for the task
    std::vector<CalendarMember> members;
    bool completed;
};

struct CalendarState
{
    std::vector<CalendarTask> tasks;
    bool showCreateModal;
    bool showEditModal;
    int editingTaskId;

    // Modal form fields
    char modalTitle[256];
    int modalDayOfWeek;
    int modalStartHour;
    int modalStartMinute;
    int modalEndHour;
    int modalEndMinute;
    char modalNotes[512];
    char modalColorTag[64];
    std::vector<CalendarMember> modalMembers;
    char newMemberName[128];

    // UI state
    int hoveredTaskId;
    int lastTaskId;

    CalendarState() : showCreateModal(false), showEditModal(false), editingTaskId(-1),
                      modalDayOfWeek(0), modalStartHour(9), modalStartMinute(0),
                      modalEndHour(10), modalEndMinute(0), hoveredTaskId(-1), lastTaskId(0)
    {
        memset(modalTitle, 0, sizeof(modalTitle));
        memset(modalNotes, 0, sizeof(modalNotes));
        memset(modalColorTag, 0, sizeof(modalColorTag));
        memset(newMemberName, 0, sizeof(newMemberName));
    }
};

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

inline ImVec4 GetColorForTag(const std::string &tag)
{
    if (tag == "Work")
        return ImVec4(0.2f, 0.5f, 1.0f, 0.8f); // Blue
    else if (tag == "Personal")
        return ImVec4(1.0f, 0.5f, 0.2f, 0.8f); // Orange
    else if (tag == "Urgent")
        return ImVec4(1.0f, 0.2f, 0.2f, 0.8f); // Red
    else if (tag == "Meeting")
        return ImVec4(0.5f, 0.2f, 1.0f, 0.8f); // Purple
    else if (tag == "Review")
        return ImVec4(0.2f, 1.0f, 0.5f, 0.8f); // Green
    return ImVec4(0.5f, 0.5f, 0.5f, 0.8f); // Gray default
}

inline const char *GetDayName(int dayOfWeek)
{
    const char *days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    return days[dayOfWeek % 7];
}

inline float GetTaskPositionY(int startHour, int startMinute)
{
    // Calculate Y position in pixels based on time
    // Assuming 9 AM to 8 PM (11 hours = 660 minutes)
    // Each pixel represents a certain time duration
    return ((startHour - 9) * 60 + startMinute) * 1.0f;
}

inline float GetTaskHeight(int startHour, int startMinute, int endHour, int endMinute)
{
    int durationMinutes = (endHour * 60 + endMinute) - (startHour * 60 + startMinute);
    return durationMinutes * 1.0f;
}

// ============================================================================
// CALENDAR UI COMPONENTS
// ============================================================================

void RenderCalendarTaskModal(CalendarState &state)
{
    ImGui::SetNextWindowSize(ImVec2(500, 600), ImGuiCond_FirstUseEver);

    if (ImGui::BeginPopupModal(state.showEditModal ? "Edit Task" : "Create Task", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::InputText("Task Title", state.modalTitle, sizeof(state.modalTitle));

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Date & Time");
        ImGui::Combo("Day of Week##Modal", &state.modalDayOfWeek, 
                     "Monday\0Tuesday\0Wednesday\0Thursday\0Friday\0Saturday\0Sunday\0", 7);

        ImGui::DragInt("Start Hour##Modal", &state.modalStartHour, 1.0f, 9, 20);
        ImGui::DragInt("Start Minute##Modal", &state.modalStartMinute, 1.0f, 0, 59);
        ImGui::DragInt("End Hour##Modal", &state.modalEndHour, 1.0f, 9, 20);
        ImGui::DragInt("End Minute##Modal", &state.modalEndMinute, 1.0f, 0, 59);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::InputText("Category/Tag", state.modalColorTag, sizeof(state.modalColorTag));
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Examples: Work, Personal, Meeting, Urgent, Review");

        ImGui::Spacing();
        ImGui::TextWrapped("Assigned Members:");

        // Display current members
        for (size_t i = 0; i < state.modalMembers.size(); ++i)
        {
            ImGui::PushID(i);
            ImGui::ColorButton("##MemberColor", 
                ImGui::ColorConvertU32ToFloat4(ImGui::GetColorU32(ImGuiCol_Button)));
            ImGui::SameLine();
            ImGui::Text("%s", state.modalMembers[i].name.c_str());
            ImGui::SameLine();

            if (ImGui::SmallButton("Remove"))
            {
                state.modalMembers.erase(state.modalMembers.begin() + i);
            }
            ImGui::PopID();
        }

        ImGui::Spacing();

        ImGui::InputText("Add Member Name", state.newMemberName, sizeof(state.newMemberName));
        ImGui::SameLine();

        if (ImGui::Button("Add Member"))
        {
            if (strlen(state.newMemberName) > 0)
            {
                CalendarMember member;
                member.name = state.newMemberName;
                member.avatarColor = "#" + std::to_string(rand() % 0xFFFFFF);
                state.modalMembers.push_back(member);
                memset(state.newMemberName, 0, sizeof(state.newMemberName));
            }
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::InputTextMultiline("Notes", state.modalNotes, sizeof(state.modalNotes), 
                                  ImVec2(-1.0f, 80.0f));

        ImGui::Spacing();

        // Buttons
        ImGui::Spacing();
        float buttonWidth = 120.0f;

        if (ImGui::Button("Save", ImVec2(buttonWidth, 0)))
        {
            if (strlen(state.modalTitle) > 0)
            {
                if (state.showEditModal && state.editingTaskId >= 0)
                {
                    // Update existing task
                    for (auto &task : state.tasks)
                    {
                        if (task.id == state.editingTaskId)
                        {
                            task.title = state.modalTitle;
                            task.dayOfWeek = state.modalDayOfWeek;
                            task.startHour = state.modalStartHour;
                            task.startMinute = state.modalStartMinute;
                            task.endHour = state.modalEndHour;
                            task.endMinute = state.modalEndMinute;
                            task.colorTag = state.modalColorTag;
                            task.color = GetColorForTag(state.modalColorTag);
                            task.notes = state.modalNotes;
                            task.members = state.modalMembers;
                            break;
                        }
                    }
                    state.showEditModal = false;
                }
                else
                {
                    // Create new task
                    CalendarTask newTask;
                    newTask.id = ++state.lastTaskId;
                    newTask.title = state.modalTitle;
                    newTask.dayOfWeek = state.modalDayOfWeek;
                    newTask.startHour = state.modalStartHour;
                    newTask.startMinute = state.modalStartMinute;
                    newTask.endHour = state.modalEndHour;
                    newTask.endMinute = state.modalEndMinute;
                    newTask.colorTag = state.modalColorTag;
                    newTask.color = GetColorForTag(state.modalColorTag);
                    newTask.notes = state.modalNotes;
                    newTask.members = state.modalMembers;
                    newTask.completed = false;

                    state.tasks.push_back(newTask);
                    state.showCreateModal = false;
                }

                state.editingTaskId = -1;
                ImGui::CloseCurrentPopup();

                // Reset form
                memset(state.modalTitle, 0, sizeof(state.modalTitle));
                memset(state.modalNotes, 0, sizeof(state.modalNotes));
                memset(state.modalColorTag, 0, sizeof(state.modalColorTag));
                state.modalMembers.clear();
                state.modalDayOfWeek = 0;
                state.modalStartHour = 9;
                state.modalStartMinute = 0;
                state.modalEndHour = 10;
                state.modalEndMinute = 0;
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0)))
        {
            state.showCreateModal = false;
            state.showEditModal = false;
            state.editingTaskId = -1;
            ImGui::CloseCurrentPopup();
            memset(state.modalTitle, 0, sizeof(state.modalTitle));
            memset(state.modalNotes, 0, sizeof(state.modalNotes));
            memset(state.modalColorTag, 0, sizeof(state.modalColorTag));
            state.modalMembers.clear();
        }

        ImGui::EndPopup();
    }
}

void RenderCalendarGrid(CalendarState &state, float cellWidth)
{
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();

    const char *dayNames[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    const int START_HOUR = 9;
    const int END_HOUR = 20;
    const float HOUR_HEIGHT = 50.0f;
    const float TIME_COLUMN_WIDTH = 60.0f;

    // Draw background
    drawList->AddRectFilled(canvasPos, 
                            ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
                            ImGui::GetColorU32(ImGuiCol_WindowBg));

    ImGui::InvisibleButton("CalendarCanvas", canvasSize);

    // Handle click to create task
    if (ImGui::IsItemClicked() && !state.showCreateModal && !state.showEditModal)
    {
        ImVec2 mousePos = ImGui::GetIO().MousePos;
        float relX = mousePos.x - canvasPos.x - TIME_COLUMN_WIDTH;
        float relY = mousePos.y - canvasPos.y - 40.0f; // header height

        if (relX > 0 && relY > 0)
        {
            int dayClicked = (int)(relX / cellWidth);
            int hourClicked = START_HOUR + (int)(relY / HOUR_HEIGHT);

            if (dayClicked >= 0 && dayClicked < 7 && hourClicked >= START_HOUR && hourClicked < END_HOUR)
            {
                state.showCreateModal = true;
                state.modalDayOfWeek = dayClicked;
                state.modalStartHour = hourClicked;
                state.modalEndHour = hourClicked + 1;
                ImGui::OpenPopup("Create Task");
            }
        }
    }

    // Draw day headers
    for (int i = 0; i < 7; ++i)
    {
        ImVec2 headerPos = ImVec2(canvasPos.x + TIME_COLUMN_WIDTH + i * cellWidth, canvasPos.y);
        ImVec2 headerEnd = ImVec2(headerPos.x + cellWidth, headerPos.y + 40.0f);

        drawList->AddRect(headerPos, headerEnd, ImGui::GetColorU32(ImGuiCol_Border));
        
        ImVec2 textPos = ImVec2(headerPos.x + 5.0f, headerPos.y + 10.0f);
        drawList->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), dayNames[i]);
    }

    // Draw time column and grid
    for (int hour = START_HOUR; hour <= END_HOUR; ++hour)
    {
        float yPos = canvasPos.y + 40.0f + (hour - START_HOUR) * HOUR_HEIGHT;

        // Time label
        char timeStr[16];
        snprintf(timeStr, sizeof(timeStr), "%02d:00", hour);
        drawList->AddText(ImVec2(canvasPos.x + 5.0f, yPos + 5.0f), 
                         ImGui::GetColorU32(ImGuiCol_Text), timeStr);

        // Grid lines
        for (int day = 0; day < 7; ++day)
        {
            float xStart = canvasPos.x + TIME_COLUMN_WIDTH + day * cellWidth;
            float xEnd = xStart + cellWidth;
            float yEnd = yPos + HOUR_HEIGHT;

            ImU32 gridColor = ImGui::GetColorU32(ImGuiCol_Border);
            drawList->AddRect(ImVec2(xStart, yPos), ImVec2(xEnd, yEnd), gridColor);
        }
    }

    // Draw current time indicator
    std::time_t now = std::time(nullptr);
    std::tm *localTime = std::localtime(&now);
    int currentHour = localTime->tm_hour;
    int currentMinute = localTime->tm_min;

    if (currentHour >= START_HOUR && currentHour < END_HOUR)
    {
        float currentY = canvasPos.y + 40.0f + ((currentHour - START_HOUR) * 60 + currentMinute) * (HOUR_HEIGHT / 60.0f);
        ImU32 currentTimeColor = ImGui::GetColorU32(ImVec4(1.0f, 0.2f, 0.2f, 1.0f)); // Red

        drawList->AddLine(ImVec2(canvasPos.x + TIME_COLUMN_WIDTH, currentY),
                         ImVec2(canvasPos.x + canvasSize.x, currentY),
                         currentTimeColor, 2.0f);

        // Circle at start of line
        drawList->AddCircleFilled(ImVec2(canvasPos.x + TIME_COLUMN_WIDTH - 5.0f, currentY),
                                 4.0f, currentTimeColor);
    }

    // Draw tasks
    std::vector<CalendarTask *> sortedTasks;
    sortedTasks.reserve(state.tasks.size());
    for (auto &task : state.tasks)
        sortedTasks.push_back(&task);

    std::sort(sortedTasks.begin(), sortedTasks.end(),
             [](const CalendarTask *a, const CalendarTask *b)
             {
                 if (a->dayOfWeek != b->dayOfWeek)
                     return a->dayOfWeek < b->dayOfWeek;
                 if (a->startHour != b->startHour)
                     return a->startHour < b->startHour;
                 return a->startMinute < b->startMinute;
             });

    // Handle overlapping tasks
    std::vector<std::vector<CalendarTask *>> columns;
    for (auto *task : sortedTasks)
    {
        bool placed = false;
        for (auto &col : columns)
        {
            bool overlaps = false;
            for (auto *existingTask : col)
            {
                if (task->dayOfWeek == existingTask->dayOfWeek)
                {
                    int taskEnd = task->startHour * 60 + task->startMinute;
                    int existingStart = existingTask->startHour * 60 + existingTask->startMinute;
                    int existingEnd = existingTask->endHour * 60 + existingTask->endMinute;
                    int taskStart = task->startHour * 60 + task->startMinute;

                    if (!(taskEnd <= existingStart || taskStart >= existingEnd))
                    {
                        overlaps = true;
                        break;
                    }
                }
            }
            if (!overlaps)
            {
                col.push_back(task);
                placed = true;
                break;
            }
        }
        if (!placed)
        {
            columns.push_back({task});
        }
    }

    // Render tasks with proper overlapping handling
    for (auto &col : columns)
    {
        int colIndex = 0;
        for (auto *task : col)
        {
            float taskX = canvasPos.x + TIME_COLUMN_WIDTH + task->dayOfWeek * cellWidth;
            float taskY = canvasPos.y + 40.0f + (task->startHour - START_HOUR) * HOUR_HEIGHT + (task->startMinute / 60.0f) * HOUR_HEIGHT;

            float duration = (task->endHour - task->startHour) * 60 + (task->endMinute - task->startMinute);
            float taskHeight = (duration / 60.0f) * HOUR_HEIGHT;
            float taskWidth = cellWidth / (float)columns.size() - 2.0f;

            taskX += colIndex * (cellWidth / (float)columns.size());

            ImVec2 taskMin = ImVec2(taskX + 1.0f, taskY + 1.0f);
            ImVec2 taskMax = ImVec2(taskX + taskWidth, taskY + taskHeight);

            // Draw task background
            drawList->AddRectFilled(taskMin, taskMax, ImGui::GetColorU32(task->color), 4.0f);
            drawList->AddRect(taskMin, taskMax, ImGui::GetColorU32(ImVec4(0.0f, 0.0f, 0.0f, 0.8f)), 4.0f);

            // Draw task title
            ImVec2 titlePos = ImVec2(taskMin.x + 5.0f, taskMin.y + 5.0f);
            drawList->AddText(titlePos, ImGui::GetColorU32(ImGuiCol_Text), task->title.c_str());

            // Draw time info
            char timeInfo[32];
            snprintf(timeInfo, sizeof(timeInfo), "%02d:%02d-%02d:%02d", 
                    task->startHour, task->startMinute, task->endHour, task->endMinute);
            ImVec2 timePos = ImVec2(taskMin.x + 5.0f, taskMin.y + 20.0f);
            drawList->AddText(timePos, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.8f)), timeInfo);

            // Draw member avatars
            float avatarY = taskMin.y + taskHeight - 20.0f;
            for (size_t i = 0; i < task->members.size() && i < 3; ++i)
            {
                float avatarX = taskMin.x + 5.0f + i * 15.0f;
                drawList->AddCircleFilled(ImVec2(avatarX, avatarY), 6.0f, 
                                         ImGui::GetColorU32(ImVec4(0.8f, 0.8f, 0.8f, 0.9f)));
                drawList->AddText(ImVec2(avatarX - 3.0f, avatarY - 5.0f), 
                                 ImGui::GetColorU32(ImGuiCol_Text), task->members[i].name.substr(0, 1).c_str());
            }

            // Handle task hover and click
            ImGui::SetCursorScreenPos(taskMin);
            ImGui::InvisibleButton(("Task" + std::to_string(task->id)).c_str(), 
                                  ImVec2(taskWidth, taskHeight));

            if (ImGui::IsItemHovered())
            {
                state.hoveredTaskId = task->id;

                // Show tooltip
                ImGui::BeginTooltip();
                ImGui::Text("Title: %s", task->title.c_str());
                ImGui::Text("Time: %02d:%02d - %02d:%02d", task->startHour, task->startMinute, 
                           task->endHour, task->endMinute);
                ImGui::Text("Category: %s", task->colorTag.c_str());
                ImGui::Text("Members: %zu", task->members.size());
                if (!task->notes.empty())
                    ImGui::TextWrapped("Notes: %s", task->notes.c_str());
                ImGui::EndTooltip();
            }

            if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
            {
                state.editingTaskId = task->id;
                state.modalDayOfWeek = task->dayOfWeek;
                state.modalStartHour = task->startHour;
                state.modalStartMinute = task->startMinute;
                state.modalEndHour = task->endHour;
                state.modalEndMinute = task->endMinute;
                strcpy(state.modalTitle, task->title.c_str());
                strcpy(state.modalNotes, task->notes.c_str());
                strcpy(state.modalColorTag, task->colorTag.c_str());
                state.modalMembers = task->members;
                state.showEditModal = true;
                ImGui::OpenPopup("Edit Task");
            }

            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                // Toggle completion
                task->completed = !task->completed;
            }

            colIndex++;
        }
    }
}

void RenderCalendarSidebar(float sidebarWidth, const char *userName)
{
    ImGui::BeginChild("Sidebar", ImVec2(sidebarWidth, 0), true, ImGuiWindowFlags_NoScrollbar);

    // User profile section
    ImGui::SetCursorPosY(15.0f);
    ImGui::SetCursorPosX(10.0f);
    ImGui::BeginGroup();

    // Avatar circle
    ImVec2 avatarPos = ImGui::GetCursorScreenPos();
    ImGui::GetWindowDrawList()->AddCircleFilled(ImVec2(avatarPos.x + 25.0f, avatarPos.y + 25.0f), 
                                               25.0f, ImGui::GetColorU32(ImVec4(0.2f, 0.5f, 1.0f, 1.0f)));
    ImGui::GetWindowDrawList()->AddText(ImVec2(avatarPos.x + 17.0f, avatarPos.y + 17.0f), 
                                        ImGui::GetColorU32(ImGuiCol_Text), "👤");

    ImGui::Dummy(ImVec2(50.0f, 50.0f));
    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::Text("Welcome");
    ImGui::TextColored(ImVec4(0.2f, 0.5f, 1.0f, 1.0f), "%s", userName);
    ImGui::EndGroup();

    ImGui::EndGroup();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Navigation
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.7f), "NAVIGATION");
    ImGui::Spacing();

    const char *navItems[] = {"📊 Dashboard", "📅 Calendar", "📁 Files", "🎯 Events", "👥 Teams", "⚙️ Settings"};
    for (const char *item : navItems)
    {
        if (ImGui::Selectable(item, false, ImGuiSelectableFlags_AllowDoubleClick))
        {
            // Navigation actions
        }
    }

    ImGui::EndChild();
}

void RenderCalendarView(CalendarState &state, const char *userName = "User")
{
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_FirstUseEver);

    ImGui::Begin("Weekly Calendar", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    float sidebarWidth = 200.0f;
    float contentWidth = windowSize.x - sidebarWidth;

    // Split layout: sidebar + calendar
    RenderCalendarSidebar(sidebarWidth, userName);
    ImGui::SameLine();

    // Calendar header with controls
    ImGui::BeginGroup();

    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.9f), "Weekly Calendar View");
    ImGui::SameLine(contentWidth - 150.0f);

    if (ImGui::Button("+ New Task", ImVec2(140.0f, 0)))
    {
        state.showCreateModal = true;
        state.modalDayOfWeek = 0;
        state.modalStartHour = 9;
        state.modalStartMinute = 0;
        state.modalEndHour = 10;
        state.modalEndMinute = 0;
        ImGui::OpenPopup("Create Task");
    }

    ImGui::Spacing();

    // Get current week dates
    time_t now = time(nullptr);
    struct tm *timeinfo = localtime(&now);
    int currentDayOfWeek = timeinfo->tm_wday; // 0 = Sunday

    // Calculate Monday of this week
    int daysFromMonday = (currentDayOfWeek + 6) % 7; // Convert Sunday=0 to Monday=0
    time_t monday = now - (daysFromMonday * 24 * 60 * 60);
    struct tm *mondayInfo = localtime(&monday);

    // Display week date range
    char dateRange[64];
    strftime(dateRange, sizeof(dateRange), "%b %d", mondayInfo);
    time_t sunday = monday + (6 * 24 * 60 * 60);
    struct tm *sundayInfo = localtime(&sunday);
    char sundayStr[32];
    strftime(sundayStr, sizeof(sundayStr), "%b %d, %Y", sundayInfo);

    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 0.8f), "Week of %s - %s", dateRange, sundayStr);

    ImGui::Spacing();
    ImGui::EndGroup();

    // Calendar grid
    float cellWidth = (contentWidth - 80.0f) / 7.0f;
    RenderCalendarGrid(state, cellWidth);

    // Task modal
    RenderCalendarTaskModal(state);

    ImGui::End();
}
