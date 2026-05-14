#pragma once

#include "calendar.h"
#include <ctime>

// ============================================================================
// EXAMPLE: Initializing Sample Tasks
// ============================================================================

void InitializeSampleTasks(CalendarState& state)
{
    // Clear existing tasks
    state.tasks.clear();
    state.lastTaskId = 0;

    // ---- Task 1: Team Standup (Monday 9:00 AM - 9:30 AM) ----
    {
        CalendarTask task;
        task.id = ++state.lastTaskId;
        task.title = "Team Standup";
        task.dayOfWeek = 0;  // Monday
        task.startHour = 9;
        task.startMinute = 0;
        task.endHour = 9;
        task.endMinute = 30;
        task.colorTag = "Meeting";
        task.color = GetColorForTag("Meeting");
        task.notes = "Daily sync with the team to discuss blockers and progress";
        
        CalendarMember alice;
        alice.name = "Alice";
        alice.avatarColor = "#FF5733";
        
        CalendarMember bob;
        bob.name = "Bob";
        bob.avatarColor = "#33B5FF";
        
        task.members.push_back(alice);
        task.members.push_back(bob);
        task.completed = false;
        
        state.tasks.push_back(task);
    }

    // ---- Task 2: Design Review (Monday 10:00 AM - 11:00 AM) ----
    {
        CalendarTask task;
        task.id = ++state.lastTaskId;
        task.title = "Design Review";
        task.dayOfWeek = 0;  // Monday
        task.startHour = 10;
        task.startMinute = 0;
        task.endHour = 11;
        task.endMinute = 0;
        task.colorTag = "Review";
        task.color = GetColorForTag("Review");
        task.notes = "Review new UI mockups for the dashboard redesign";
        
        CalendarMember charlie;
        charlie.name = "Charlie";
        charlie.avatarColor = "#33FF57";
        
        CalendarMember alice;
        alice.name = "Alice";
        alice.avatarColor = "#FF5733";
        
        task.members.push_back(charlie);
        task.members.push_back(alice);
        task.completed = false;
        
        state.tasks.push_back(task);
    }

    // ---- Task 3: Code Review - Overlapping (Monday 10:30 AM - 11:30 AM) ----
    {
        CalendarTask task;
        task.id = ++state.lastTaskId;
        task.title = "Code Review";
        task.dayOfWeek = 0;  // Monday - overlaps with Design Review
        task.startHour = 10;
        task.startMinute = 30;
        task.endHour = 11;
        task.endMinute = 30;
        task.colorTag = "Work";
        task.color = GetColorForTag("Work");
        task.notes = "Review Pull Request #345 - Authentication module";
        
        CalendarMember bob;
        bob.name = "Bob";
        bob.avatarColor = "#33B5FF";
        
        task.members.push_back(bob);
        task.completed = false;
        
        state.tasks.push_back(task);
    }

    // ---- Task 4: Lunch Break (Tuesday 12:00 PM - 1:00 PM) ----
    {
        CalendarTask task;
        task.id = ++state.lastTaskId;
        task.title = "Lunch Break";
        task.dayOfWeek = 1;  // Tuesday
        task.startHour = 12;
        task.startMinute = 0;
        task.endHour = 13;
        task.endMinute = 0;
        task.colorTag = "Personal";
        task.color = GetColorForTag("Personal");
        task.notes = "Team lunch at downtown restaurant";
        
        task.completed = false;
        state.tasks.push_back(task);
    }

    // ---- Task 5: Client Presentation - URGENT (Wednesday 2:00 PM - 3:30 PM) ----
    {
        CalendarTask task;
        task.id = ++state.lastTaskId;
        task.title = "Client Presentation";
        task.dayOfWeek = 2;  // Wednesday
        task.startHour = 14;
        task.startMinute = 0;
        task.endHour = 15;
        task.endMinute = 30;
        task.colorTag = "Urgent";
        task.color = GetColorForTag("Urgent");
        task.notes = "Present Q2 roadmap to stakeholders. Bring slides and demo.";
        
        CalendarMember alice;
        alice.name = "Alice";
        alice.avatarColor = "#FF5733";
        
        CalendarMember bob;
        bob.name = "Bob";
        bob.avatarColor = "#33B5FF";
        
        CalendarMember david;
        david.name = "David";
        david.avatarColor = "#FFD700";
        
        task.members.push_back(alice);
        task.members.push_back(bob);
        task.members.push_back(david);
        task.completed = false;
        
        state.tasks.push_back(task);
    }

    // ---- Task 6: 1:1 Meeting (Thursday 3:00 PM - 3:30 PM) ----
    {
        CalendarTask task;
        task.id = ++state.lastTaskId;
        task.title = "1:1 with Manager";
        task.dayOfWeek = 3;  // Thursday
        task.startHour = 15;
        task.startMinute = 0;
        task.endHour = 15;
        task.endMinute = 30;
        task.colorTag = "Meeting";
        task.color = GetColorForTag("Meeting");
        task.notes = "Discuss performance review and career growth opportunities";
        
        task.completed = false;
        state.tasks.push_back(task);
    }

    // ---- Task 7: Bug Fixing (Friday 10:00 AM - 12:00 PM) ----
    {
        CalendarTask task;
        task.id = ++state.lastTaskId;
        task.title = "Bug Fixing Sprint";
        task.dayOfWeek = 4;  // Friday
        task.startHour = 10;
        task.startMinute = 0;
        task.endHour = 12;
        task.endMinute = 0;
        task.colorTag = "Work";
        task.color = GetColorForTag("Work");
        task.notes = "Fix critical bugs from the backlog: #123, #124, #125";
        
        CalendarMember charlie;
        charlie.name = "Charlie";
        charlie.avatarColor = "#33FF57";
        
        task.members.push_back(charlie);
        task.completed = false;
        
        state.tasks.push_back(task);
    }

    // ---- Task 8: Sprint Planning (Friday 2:00 PM - 3:00 PM) ----
    {
        CalendarTask task;
        task.id = ++state.lastTaskId;
        task.title = "Sprint Planning";
        task.dayOfWeek = 4;  // Friday
        task.startHour = 14;
        task.startMinute = 0;
        task.endHour = 15;
        task.endMinute = 0;
        task.colorTag = "Meeting";
        task.color = GetColorForTag("Meeting");
        task.notes = "Plan tasks for next sprint. Estimate story points.";
        
        CalendarMember alice;
        alice.name = "Alice";
        alice.avatarColor = "#FF5733";
        
        CalendarMember bob;
        bob.name = "Bob";
        bob.avatarColor = "#33B5FF";
        
        CalendarMember charlie;
        charlie.name = "Charlie";
        charlie.avatarColor = "#33FF57";
        
        task.members.push_back(alice);
        task.members.push_back(bob);
        task.members.push_back(charlie);
        task.completed = false;
        
        state.tasks.push_back(task);
    }

    // ---- Task 9: Documentation (Saturday 10:00 AM - 11:00 AM) ----
    {
        CalendarTask task;
        task.id = ++state.lastTaskId;
        task.title = "Documentation Writing";
        task.dayOfWeek = 5;  // Saturday
        task.startHour = 10;
        task.startMinute = 0;
        task.endHour = 11;
        task.endMinute = 0;
        task.colorTag = "Personal";
        task.color = GetColorForTag("Personal");
        task.notes = "Write documentation for calendar component. API docs needed.";
        
        task.completed = false;
        state.tasks.push_back(task);
    }

    // ---- Task 10: Review & Testing (Sunday 4:00 PM - 5:00 PM) ----
    {
        CalendarTask task;
        task.id = ++state.lastTaskId;
        task.title = "Testing & QA";
        task.dayOfWeek = 6;  // Sunday
        task.startHour = 16;
        task.startMinute = 0;
        task.endHour = 17;
        task.endMinute = 0;
        task.colorTag = "Review";
        task.color = GetColorForTag("Review");
        task.notes = "Final QA testing before production release. Check all browsers.";
        
        CalendarMember charlie;
        charlie.name = "Charlie";
        charlie.avatarColor = "#33FF57";
        
        task.members.push_back(charlie);
        task.completed = false;
        
        state.tasks.push_back(task);
    }
}

// ============================================================================
// EXAMPLE: Programmatically Creating a Task
// ============================================================================

CalendarTask CreateTask(
    CalendarState& state,
    const std::string& title,
    int dayOfWeek,
    int startHour,
    int startMinute,
    int endHour,
    int endMinute,
    const std::string& colorTag,
    const std::string& notes = "")
{
    CalendarTask task;
    task.id = ++state.lastTaskId;
    task.title = title;
    task.dayOfWeek = dayOfWeek;
    task.startHour = startHour;
    task.startMinute = startMinute;
    task.endHour = endHour;
    task.endMinute = endMinute;
    task.colorTag = colorTag;
    task.color = GetColorForTag(colorTag);
    task.notes = notes;
    task.completed = false;
    
    state.tasks.push_back(task);
    return task;
}

// ============================================================================
// EXAMPLE: Adding Members to a Task
// ============================================================================

void AddMemberToTask(CalendarTask& task, const std::string& name, const std::string& colorHex)
{
    CalendarMember member;
    member.name = name;
    member.avatarColor = colorHex;
    task.members.push_back(member);
}

// ============================================================================
// EXAMPLE: Finding and Modifying Tasks
// ============================================================================

CalendarTask* FindTaskById(CalendarState& state, int taskId)
{
    for (auto& task : state.tasks)
    {
        if (task.id == taskId)
            return &task;
    }
    return nullptr;
}

std::vector<CalendarTask*> FindTasksByDay(CalendarState& state, int dayOfWeek)
{
    std::vector<CalendarTask*> results;
    for (auto& task : state.tasks)
    {
        if (task.dayOfWeek == dayOfWeek)
            results.push_back(&task);
    }
    return results;
}

std::vector<CalendarTask*> FindTasksByCategory(CalendarState& state, const std::string& colorTag)
{
    std::vector<CalendarTask*> results;
    for (auto& task : state.tasks)
    {
        if (task.colorTag == colorTag)
            results.push_back(&task);
    }
    return results;
}

// ============================================================================
// EXAMPLE: Deleting a Task
// ============================================================================

bool DeleteTask(CalendarState& state, int taskId)
{
    for (auto it = state.tasks.begin(); it != state.tasks.end(); ++it)
    {
        if (it->id == taskId)
        {
            state.tasks.erase(it);
            return true;
        }
    }
    return false;
}

// ============================================================================
// EXAMPLE: Calculating Task Duration
// ============================================================================

int GetTaskDurationMinutes(const CalendarTask& task)
{
    int startMinutes = task.startHour * 60 + task.startMinute;
    int endMinutes = task.endHour * 60 + task.endMinute;
    return endMinutes - startMinutes;
}

// ============================================================================
// EXAMPLE: Checking for Conflicts
// ============================================================================

bool CheckTimeConflict(const CalendarTask& task1, const CalendarTask& task2)
{
    // Only check if on same day
    if (task1.dayOfWeek != task2.dayOfWeek)
        return false;
    
    int task1Start = task1.startHour * 60 + task1.startMinute;
    int task1End = task1.endHour * 60 + task1.endMinute;
    int task2Start = task2.startHour * 60 + task2.startMinute;
    int task2End = task2.endHour * 60 + task2.endMinute;
    
    // Check overlap
    return !(task1End <= task2Start || task1Start >= task2End);
}

std::vector<CalendarTask*> FindConflictingTasks(CalendarState& state, const CalendarTask& task)
{
    std::vector<CalendarTask*> conflicts;
    for (auto& existingTask : state.tasks)
    {
        if (existingTask.id != task.id && CheckTimeConflict(task, existingTask))
        {
            conflicts.push_back(&existingTask);
        }
    }
    return conflicts;
}

// ============================================================================
// EXAMPLE: Serialization (Simple JSON-like format)
// ============================================================================

std::string SerializeTaskToJson(const CalendarTask& task)
{
    // Simple string serialization (can be enhanced with JSON library)
    std::string json = "{\n";
    json += "  \"id\": " + std::to_string(task.id) + ",\n";
    json += "  \"title\": \"" + task.title + "\",\n";
    json += "  \"dayOfWeek\": " + std::to_string(task.dayOfWeek) + ",\n";
    json += "  \"startHour\": " + std::to_string(task.startHour) + ",\n";
    json += "  \"startMinute\": " + std::to_string(task.startMinute) + ",\n";
    json += "  \"endHour\": " + std::to_string(task.endHour) + ",\n";
    json += "  \"endMinute\": " + std::to_string(task.endMinute) + ",\n";
    json += "  \"colorTag\": \"" + task.colorTag + "\",\n";
    json += "  \"notes\": \"" + task.notes + "\",\n";
    json += "  \"completed\": " + std::string(task.completed ? "true" : "false") + "\n";
    json += "}";
    return json;
}

// ============================================================================
// EXAMPLE: Statistics
// ============================================================================

struct CalendarStatistics
{
    int totalTasks;
    int completedTasks;
    int tasksThisWeek;
    float averageTaskDuration;
    int mostCommonCategory;
};

CalendarStatistics GetCalendarStatistics(const CalendarState& state)
{
    CalendarStatistics stats = {0, 0, 0, 0.0f, 0};
    
    stats.totalTasks = state.tasks.size();
    
    int totalDuration = 0;
    for (const auto& task : state.tasks)
    {
        if (task.completed)
            stats.completedTasks++;
        
        totalDuration += GetTaskDurationMinutes(task);
    }
    
    if (stats.totalTasks > 0)
    {
        stats.averageTaskDuration = (float)totalDuration / stats.totalTasks;
    }
    
    stats.tasksThisWeek = stats.totalTasks;  // Assuming all tasks are in current week
    
    return stats;
}

// ============================================================================
// EXAMPLE: Usage in Main Application
// ============================================================================

/*
// In main.cpp, before rendering loop:

// Initialize calendar with sample tasks
InitializeSampleTasks(calendarState);

// Or programmatically:
CreateTask(calendarState, "Morning Standup", 0, 9, 0, 9, 30, "Meeting", "Daily sync");

// In render loop:
if (currentScreen == SCREEN_CALENDAR)
{
    RenderCalendarView(calendarState, "Ahmed");
    
    // Optionally display statistics
    CalendarStatistics stats = GetCalendarStatistics(calendarState);
    ImGui::Text("Total Tasks: %d, Completed: %d", stats.totalTasks, stats.completedTasks);
}
*/

#endif // CALENDAR_EXAMPLES_H
