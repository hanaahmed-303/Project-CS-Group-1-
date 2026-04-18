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
// ------------------------------------------------------------
// Load background texture
// ------------------------------------------------------------
enum class EventType
{
    None,
    Wedding,
    Engagement,
    Birthday,
    Corporate
};
static EventType selectedEvent = EventType::None;

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

    if (ImGui::Button("Corporate Event", ImVec2(200.0f, 60.0f)))
        selectedEvent = EventType::Corporate;

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Plan a corporate meeting or conference");

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

    style.WindowRounding = 15.0f;
    style.FrameRounding = 10.0f;

    ImVec4 *colors = style.Colors;

    colors[ImGuiCol_WindowBg] = ImVec4(1.0f, 0.9f, 0.95f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(1.0f, 0.4f, 0.7f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.5f, 0.8f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 0.3f, 0.6f, 1.0f);
    colors[ImGuiCol_Text] = ImVec4(0.6f, 0.0f, 0.3f, 1.0f);
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
        1280,
        720,
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

    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    SetPinkTheme();

    GLuint background = LoadTexture("confetti.png");

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (background != 0)
        {
            ImGui::GetBackgroundDrawList()->AddImage(
                (void *)(intptr_t)background,
                ImVec2(0, 0),
                ImVec2(
                    ImGui::GetIO().DisplaySize.x,
                    ImGui::GetIO().DisplaySize.y));
        }

        float window_width = 600;
        float window_height = 420;

        ImGui::SetNextWindowPos(
            ImVec2(
                (io.DisplaySize.x - window_width) * 0.5f,
                (io.DisplaySize.y - window_height) * 0.5f),
            ImGuiCond_Always);

        ImGui::SetNextWindowSize(
            ImVec2(window_width, window_height),
            ImGuiCond_Always);

        // ================= LOGIN =================

        if (currentScreen == SCREEN_LOGIN)
        {
            ImGui::Begin("Welcome to Eventopia", NULL,
                         ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoCollapse);

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
                         ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoCollapse);

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
                         ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoCollapse);

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
            ImGui::Begin("Next Step");

            ImGui::Text("Next page: Budget, Mood, Tasks, etc.");

            if (ImGui::Button("Back", ImVec2(120.0f, 40.0f)))
            {
                currentScreen = SCREEN_EVENT_TYPE;
            }

            ImGui::End();
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
