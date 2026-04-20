#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cstdio>
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>

enum class EventType
{
    None,
    Party,
    Wedding,
    Birthday,
    Conference,
    Engagement
};
EventType selectedEvent = EventType::None;
////////////////////////////////////////////////////////////
// PUT ALL VARIABLES HERE
////////////////////////////////////////////////////////////

// ---------------- BIRTHDAY ----------------

char birthdayTheme[100] = "";
float birthdayBudget = 0.0f;
char birthdayGuestList[200] = "";
char birthdayDateTime[50] = "";
char birthdayInvitations[100] = "";
char birthdayVenue[100] = "";
char birthdayCake[100] = "";
char birthdayDecorations[100] = "";
char birthdayFoodDrinks[100] = "";
char birthdayPlaylist[100] = "";
char birthdayRSVPs[100] = "";
char birthdaySetupVenue[100] = "";
char birthdayDecorateSpace[100] = "";
char birthdayFoodTable[100] = "";
char birthdayMusic[100] = "";
char birthdayPhotos[100] = "";

// ---------------- WEDDING ----------------

float weddingBudget = 0.0f;
char weddingDate[50] = "";
char weddingVenue[100] = "";
char weddingGuestList[200] = "";
char weddingPhotographer[100] = "";
char weddingVideographer[100] = "";
char weddingCatering[100] = "";
char weddingInvitations[100] = "";
char weddingOutfits[100] = "";
char weddingMakeupArtist[100] = "";
char weddingSeatingArrangement[100] = "";
char weddingFlowers[100] = "";
char weddingTransport[100] = "";
char weddingVendors[100] = "";
char weddingSchedule[100] = "";
char weddingSetupVenue[100] = "";
char weddingCeremony[100] = "";
char weddingTimeline[100] = "";

// ---------------- PARTY ----------------

char partyTheme[100] = "";
float partyBudget = 0.0f;
char partyGuestList[200] = "";
char partyDateTime[50] = "";
char partyInvitations[100] = "";
char partyFoodDrinks[100] = "";
char partySnacks[100] = "";
char partyDecorations[100] = "";
char partyPlaylist[100] = "";
char partySetupSpace[100] = "";
char partySeating[100] = "";
char partyMusicSystem[100] = "";
char partyWelcomeGuests[100] = "";
char partyServeFood[100] = "";
char partyPhotos[100] = "";
char partyCleanup[100] = "";
// ---------------- ENGAGEMENT ----------------

float engagementBudget = 0.0f;
char engagementDate[50] = "";
char engagementVenue[100] = "";
char engagementGuestList[200] = "";
char engagementInvitations[100] = "";
char engagementRings[100] = "";
char engagementDecorations[100] = "";
char engagementCatering[100] = "";
char engagementMusic[100] = "";
char engagementVendors[100] = "";
char engagementSchedule[100] = "";

// ---------------- CONFERENCE ----------------

char conferenceTopic[100] = "";
float conferenceBudget = 0.0f;
char conferenceVenue[100] = "";
char conferenceAgenda[100] = "";
char conferenceSpeakers[200] = "";
char conferenceRegistration[100] = "";
char conferenceConfirmSpeakers[100] = "";
char conferenceEquipment[100] = "";
char conferenceWiFi[100] = "";
char conferencePromotion[100] = "";
char conferenceBadges[100] = "";
char conferenceStaffRoles[100] = "";
char conferenceCatering[100] = "";
char conferenceSchedule[100] = "";
char conferenceSetupVenue[100] = "";
char conferenceCheckIn[100] = "";
char conferenceSessions[100] = "";
char conferenceTechnicalIssues[100] = "";

std::vector<const char *> birthdayTasks = {
    "Choose theme",
    "Set budget",
    "Create guest list",
    "Pick date and time",
    "Send invitations",
    "Book venue",
    "Order cake",
    "Buy decorations",
    "Plan food and drinks",
    "Prepare playlist",
    "Confirm RSVPs",
    "Decorate space",
    "Play music",
    "Take photos"};

std::vector<const char *> weddingTasks = {
    "Set budget",
    "Choose wedding date",
    "Book venue",
    "Create guest list",
    "Hire photographer",
    "Hire videographer",
    "Book catering",
    "Send invitations",
    "Choose wedding outfits",
    "Book makeup artist",
    "Plan seating arrangement",
    "Order flowers",
    "Arrange transport",
    "Confirm vendors",
    "Final event schedule"};

std::vector<const char *> partyTasks = {
    "Choose theme",
    "Set budget",
    "Create guest list",
    "Pick date and time",
    "Send invitations",
    "Plan food and drinks",
    "Buy snacks and drinks",
    "Get decorations",
    "Prepare playlist",
    "Arrange seating",
    "Serve food and drinks",
    "Take photos",
    "Clean up after party"};

std::vector<const char *> conferenceTasks = {
    "Define topic",
    "Set budget",
    "Book venue",
    "Create agenda",
    "Invite speakers",
    "Open registration",
    "Arrange equipment",
    "Set up WiFi",
    "Promote event",
    "Print badges",
    "Assign staff roles",
    "Confirm catering",
    "Final schedule check",
    "Manage check-in"};
std::vector<const char *> engagementTasks = {
    "Set budget",
    "Choose date",
    "Book venue",
    "Create guest list",
    "Send invitations",
    "Order rings",
    "Plan decorations",
    "Arrange catering",
    "Prepare music",
    "Confirm vendors",
    "Finalize schedule"};
// ------------------------------------------------------------
// ------------------------------------------------------------

enum Screen
{
    SCREEN_LOGIN,
    SCREEN_CREATE_ACCOUNT,
    SCREEN_APP,
    SCREEN_EVENT_TYPE,
    SCREEN_EVENT_DETAILS

};
Screen currentScreen = SCREEN_LOGIN;

void RenderEventTypePage()
{
    ImGui::Begin("Create Event - Select Type");

    ImGui::Text("Choose your event type:");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.0f, 10.0f));

    if (ImGui::Button("Wedding", ImVec2(200.0f, 60.0f)))
        selectedEvent = EventType::Wedding;

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Plan a full wedding event");

    if (ImGui::Button("Engagement", ImVec2(200.0f, 60.0f)))
        selectedEvent = EventType::Engagement;

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Organize an engagement ceremony");

    if (ImGui::Button("Birthday Party", ImVec2(200.0f, 60.0f)))
        selectedEvent = EventType::Birthday;

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Plan a birthday party");

    if (ImGui::Button("Party", ImVec2(200.0f, 60.0f)))
        selectedEvent = EventType::Party;

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Plan a general party");
    if (ImGui::Button("Conference", ImVec2(200.0f, 60.0f)))
        selectedEvent = EventType::Conference;

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Plan a corporate meeting or conference");
    std::vector<const char *> *tasks = nullptr;

    if (selectedEvent == EventType::Birthday)
        tasks = &birthdayTasks;

    else if (selectedEvent == EventType::Wedding)
        tasks = &weddingTasks;

    else if (selectedEvent == EventType::Party)
        tasks = &partyTasks;
    else if (selectedEvent == EventType::Engagement)
        tasks = &engagementTasks;
    else if (selectedEvent == EventType::Conference)
        tasks = &conferenceTasks;

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (selectedEvent != EventType::None)
    {
        ImGui::Text("Event selected!");
        ImGui::Spacing();

        if (ImGui::Button("Next ->", ImVec2(140.0f, 40.0f)))
        {
            currentScreen = SCREEN_EVENT_DETAILS;
        }
    }

    if (ImGui::Button("Back to Login", ImVec2(160.0f, 35.0f)))
    {
        currentScreen = SCREEN_LOGIN;
    }

    ImGui::PopStyleVar(2);

    ImGui::End();
}

// ------------------------------------------------------------
// ACCOUNT FUNCTIONS
// ------------------------------------------------------------

bool accountExists(const char *username, const char *password)
{
    std::ifstream file("accounts.txt");

    if (!file.is_open())
        return false;

    std::string line;

    while (std::getline(file, line))
    {
        size_t commaPos = line.find(',');

        if (commaPos == std::string::npos)
            continue;

        std::string user = line.substr(0, commaPos);
        std::string pass = line.substr(commaPos + 1);

        if (user == username && pass == password)
        {
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

void saveAccount(const char *username, const char *password)
{
    std::ofstream file("accounts.txt", std::ios::app);

    if (!file.is_open())
        return;

    // Save using comma separator so spaces work in usernames
    file << username << "," << password << std::endl;

    file.close();
}

// ------------------------------------------------------------
// Load background texture
// ------------------------------------------------------------

GLuint LoadTexture(const char *filename)
{
    int width, height, channels;

    unsigned char *data =
        stbi_load(filename, &width, &height,
                  &channels, 4);

    if (!data)
    {
        printf("Failed to load image: %s\n", filename);
        return 0;
    }

    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR);

    stbi_image_free(data);

    return texture;
}

// ------------------------------------------------------------
// Pink theme styling
// ------------------------------------------------------------

void SetPinkTheme()
{
    ImGuiStyle &style = ImGui::GetStyle();

    style.WindowRounding = 18.0f;
    style.FrameRounding = 12.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabRounding = 10.0f;

    ImVec4 *colors = style.Colors;

    colors[ImGuiCol_WindowBg] = ImVec4(1.0f, 0.93f, 0.96f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(1.0f, 0.55f, 0.75f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.65f, 0.82f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 0.45f, 0.70f, 1.0f);

    colors[ImGuiCol_FrameBg] = ImVec4(1.0f, 0.85f, 0.90f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(1.0f, 0.80f, 0.88f, 1.0f);

    colors[ImGuiCol_Text] = ImVec4(0.55f, 0.0f, 0.30f, 1.0f);
}
void RenderEventDetailsPage()
{
    ImGui::Begin("Event Details");

    // ================= BIRTHDAY =================
    if (selectedEvent == EventType::Birthday)
    {
        ImGui::Text("Birthday Event Details");
        ImGui::Separator();

        ImGui::InputText("Theme", birthdayTheme, 100);
        ImGui::InputFloat("Budget", &birthdayBudget);
        ImGui::InputText("Guest List", birthdayGuestList, 200);
        ImGui::InputText("Date and Time", birthdayDateTime, 50);
        ImGui::InputText("Invitations", birthdayInvitations, 100);
        ImGui::InputText("Venue", birthdayVenue, 100);
        ImGui::InputText("Cake", birthdayCake, 100);
        ImGui::InputText("Decorations", birthdayDecorations, 100);
        ImGui::InputText("Food and Drinks", birthdayFoodDrinks, 100);
        ImGui::InputText("Playlist", birthdayPlaylist, 100);
        ImGui::InputText("RSVPs", birthdayRSVPs, 100);
        ImGui::InputText("Setup Venue", birthdaySetupVenue, 100);
        ImGui::InputText("Decorate Space", birthdayDecorateSpace, 100);
        ImGui::InputText("Food Table", birthdayFoodTable, 100);
        ImGui::InputText("Music", birthdayMusic, 100);
        ImGui::InputText("Photos", birthdayPhotos, 100);
    }

    // ================= WEDDING =================
    if (selectedEvent == EventType::Wedding)
    {
        ImGui::Text("Wedding Event Details");
        ImGui::Separator();

        ImGui::InputFloat("Budget", &weddingBudget);
        ImGui::InputText("Wedding Date", weddingDate, 50);
        ImGui::InputText("Venue", weddingVenue, 100);
        ImGui::InputText("Guest List", weddingGuestList, 200);
        ImGui::InputText("Photographer", weddingPhotographer, 100);
        ImGui::InputText("Videographer", weddingVideographer, 100);
        ImGui::InputText("Catering", weddingCatering, 100);
        ImGui::InputText("Invitations", weddingInvitations, 100);
        ImGui::InputText("Outfits", weddingOutfits, 100);
        ImGui::InputText("Makeup Artist", weddingMakeupArtist, 100);
        ImGui::InputText("Seating Arrangement", weddingSeatingArrangement, 100);
        ImGui::InputText("Flowers", weddingFlowers, 100);
        ImGui::InputText("Transport", weddingTransport, 100);
        ImGui::InputText("Vendors", weddingVendors, 100);
        ImGui::InputText("Schedule", weddingSchedule, 100);
    }

    // ================= PARTY =================
    if (selectedEvent == EventType::Party)
    {
        ImGui::Text("Party Event Details");
        ImGui::Separator();

        ImGui::InputText("Theme", partyTheme, 100);
        ImGui::InputFloat("Budget", &partyBudget);
        ImGui::InputText("Guest List", partyGuestList, 200);
        ImGui::InputText("Date and Time", partyDateTime, 50);
        ImGui::InputText("Invitations", partyInvitations, 100);
        ImGui::InputText("Food and Drinks", partyFoodDrinks, 100);
        ImGui::InputText("Snacks", partySnacks, 100);
        ImGui::InputText("Decorations", partyDecorations, 100);
        ImGui::InputText("Playlist", partyPlaylist, 100);
        ImGui::InputText("Setup Space", partySetupSpace, 100);
        ImGui::InputText("Seating", partySeating, 100);
        ImGui::InputText("Music System", partyMusicSystem, 100);
        ImGui::InputText("Welcome Guests", partyWelcomeGuests, 100);
        ImGui::InputText("Serve Food", partyServeFood, 100);
        ImGui::InputText("Photos", partyPhotos, 100);
        ImGui::InputText("Cleanup", partyCleanup, 100);
    }
    // ================= Engagement =================

    if (selectedEvent == EventType::Engagement)
    {
        ImGui::Text("Engagement Event Details");
        ImGui::Separator();

        ImGui::InputFloat("Budget", &engagementBudget);
        ImGui::InputText("Date", engagementDate, 50);
        ImGui::InputText("Venue", engagementVenue, 100);
        ImGui::InputText("Guest List", engagementGuestList, 200);
        ImGui::InputText("Invitations", engagementInvitations, 100);
        ImGui::InputText("Rings", engagementRings, 100);
        ImGui::InputText("Decorations", engagementDecorations, 100);
        ImGui::InputText("Catering", engagementCatering, 100);
        ImGui::InputText("Music", engagementMusic, 100);
        ImGui::InputText("Vendors", engagementVendors, 100);
        ImGui::InputText("Schedule", engagementSchedule, 100);
    }

    // ================= CONFERENCE =================
    if (selectedEvent == EventType::Conference)
    {
        ImGui::Text("Conference Event Details");
        ImGui::Separator();

        ImGui::InputText("Topic", conferenceTopic, 100);
        ImGui::InputFloat("Budget", &conferenceBudget);
        ImGui::InputText("Venue", conferenceVenue, 100);
        ImGui::InputText("Agenda", conferenceAgenda, 100);
        ImGui::InputText("Speakers", conferenceSpeakers, 200);
        ImGui::InputText("Registration", conferenceRegistration, 100);
        ImGui::InputText("Confirm Speakers", conferenceConfirmSpeakers, 100);
        ImGui::InputText("Equipment", conferenceEquipment, 100);
        ImGui::InputText("WiFi", conferenceWiFi, 100);
        ImGui::InputText("Promotion", conferencePromotion, 100);
        ImGui::InputText("Badges", conferenceBadges, 100);
        ImGui::InputText("Staff Roles", conferenceStaffRoles, 100);
        ImGui::InputText("Catering", conferenceCatering, 100);
        ImGui::InputText("Schedule", conferenceSchedule, 100);
    }

    ImGui::Spacing();

    if (ImGui::Button("Back"))
    {
        currentScreen = SCREEN_EVENT_TYPE;
    }

    ImGui::End();
}
// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------

int main()
{

    if (!glfwInit())
        return 1;

    const char *glsl_version = "#version 150";

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(
        1920,
        1080,
        "Eventopia",
        NULL,
        NULL);
    if (window == NULL)
    {
        printf("Window creation failed\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    SetPinkTheme();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        float window_width = io.DisplaySize.x;
        float window_height = io.DisplaySize.y;
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);

        // ================= LOGIN =================

        if (currentScreen == SCREEN_LOGIN)
        {
            ImGui::Begin("Welcome to Eventopia", NULL,
                         ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoTitleBar);

            static char username[50] = "";
            static char password[50] = "";

            static std::string loginMessage = "";

            float field_width = 260;

            ImGui::Text("Login to continue");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::SetCursorPosX(
                (window_width - field_width) * 0.5f);

            ImGui::PushItemWidth(field_width);
            ImGui::InputText("Username", username, 50);
            ImGui::PopItemWidth();

            ImGui::Spacing();

            ImGui::SetCursorPosX(
                (window_width - field_width) * 0.5f);

            ImGui::PushItemWidth(field_width);
            ImGui::InputText(
                "Password",
                password,
                50,
                ImGuiInputTextFlags_Password);
            ImGui::PopItemWidth();

            ImGui::Spacing();

            bool canLogin =
                strlen(username) > 0 &&
                strlen(password) > 0;

            ImGui::SetCursorPosX(
                (window_width - field_width) * 0.5f);

            if (!canLogin)
                ImGui::BeginDisabled();

            if (ImGui::Button(
                    "Login",
                    ImVec2(field_width, 50)))
            {
                if (accountExists(username, password))
                {
                    loginMessage = "Login successful!";
                    currentScreen = SCREEN_APP;
                }
                else
                {
                    loginMessage =
                        "Incorrect username or password.";
                }
            }

            if (!canLogin)
                ImGui::EndDisabled();

            ImGui::Spacing();

            ImGui::SetCursorPosX(
                (window_width - field_width) * 0.5f);

            if (ImGui::Button(
                    "Create Account",
                    ImVec2(field_width, 50)))
            {
                currentScreen = SCREEN_CREATE_ACCOUNT;
            }

            ImGui::Spacing();

            ImGui::Text("%s", loginMessage.c_str());

            ImGui::End();
        }

        // ================= CREATE ACCOUNT =================

        if (currentScreen == SCREEN_CREATE_ACCOUNT)
        {
            ImGui::Begin("Create Account", NULL,
                         ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoTitleBar);

            static char newUsername[50] = "";
            static char newPassword[50] = "";

            static std::string message = "";

            float field_width = 260;

            ImGui::Text("Create your account");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::SetCursorPosX(
                (window_width - field_width) * 0.5f);

            ImGui::PushItemWidth(field_width);
            ImGui::InputText(
                "New Username",
                newUsername,
                50);
            ImGui::PopItemWidth();

            ImGui::Spacing();

            ImGui::SetCursorPosX(
                (window_width - field_width) * 0.5f);

            ImGui::PushItemWidth(field_width);
            ImGui::InputText(
                "New Password",
                newPassword,
                50,
                ImGuiInputTextFlags_Password);
            ImGui::PopItemWidth();

            ImGui::Spacing();

            bool canCreate =
                strlen(newUsername) > 0 &&
                strlen(newPassword) > 0;

            ImGui::SetCursorPosX(
                (window_width - field_width) * 0.5f);

            if (!canCreate)
                ImGui::BeginDisabled();

            if (ImGui::Button(
                    "Create Account",
                    ImVec2(field_width, 50)))
            {
                saveAccount(newUsername, newPassword);

                message =
                    "Account created successfully!";

                strcpy(newUsername, "");
                strcpy(newPassword, "");
            }

            if (!canCreate)
                ImGui::EndDisabled();

            ImGui::Spacing();

            ImGui::SetCursorPosX(
                (window_width - field_width) * 0.5f);

            if (ImGui::Button(
                    "Back to Login",
                    ImVec2(field_width, 50)))
            {
                currentScreen = SCREEN_LOGIN;
            }

            ImGui::Spacing();

            ImGui::Text("%s", message.c_str());

            ImGui::End();
        }

        // ================= APP =================

        if (currentScreen == SCREEN_APP)
        {
            ImGui::Begin("Event Creator", NULL,
                         ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoTitleBar);

            ImGui::Text(
                "Welcome to the Event Planning System!");

            ImGui::Spacing();

            ImGui::SetCursorPosX(
                (window_width - 260) * 0.5f);

            if (ImGui::Button(
                    "Create Event",
                    ImVec2(260, 60)))
            {
                currentScreen = SCREEN_EVENT_TYPE;
            }

            ImGui::End();
        }
        else if (currentScreen == SCREEN_EVENT_TYPE)
        {
            RenderEventTypePage();
        }
        else if (currentScreen == SCREEN_EVENT_DETAILS)
        {
            RenderEventDetailsPage();
        }

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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
