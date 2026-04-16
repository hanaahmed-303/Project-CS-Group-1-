#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cstdio>
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>


// ------------------------------------------------------------
// Load background texture
// ------------------------------------------------------------
GLuint LoadTexture(const char* filename)
{
    int width, height, channels;

    unsigned char* data =
        stbi_load(filename, &width, &height,
                  &channels, 4); // force RGBA

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
        data
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

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

int main()
{
    // ------------------------------------------------------------
    // 1. Initialize GLFW
    // ------------------------------------------------------------

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

    // ------------------------------------------------------------
    // 2. Setup ImGui
    // ------------------------------------------------------------

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    // Initialize backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Apply theme
    SetPinkTheme();

    // Load background AFTER OpenGL is ready
    GLuint background = LoadTexture("confetti.png");

    // ------------------------------------------------------------
    // 3. Render loop
    // ------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Background image
        if (background != 0)
        {
            ImGui::GetBackgroundDrawList()->AddImage(
                (void*)(intptr_t)background,
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

        ImGui::Begin(
            "Welcome to Eventopia",
            NULL,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse);

        ImGui::SetCursorPosX(
            (window_width -
             ImGui::CalcTextSize("Welcome to Eventopia!").x) *
            0.5f);

        ImGui::Text("Welcome to Eventopia!");

        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text(
            "Eventopia helps you organize events like:");

        ImGui::BulletText("Weddings");
        ImGui::BulletText("Birthdays");
        ImGui::BulletText("Parties");
        ImGui::BulletText("Conferences");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Founders / Programmers:");
        ImGui::BulletText("Your Name");
        ImGui::BulletText("Team Member");

        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::SetCursorPosX(
            (window_width - 260) * 0.5f);

        if (ImGui::Button(
                "Create Account / Login",
                ImVec2(260, 50)))
        {
            printf("Login clicked\n");
        }

        ImGui::Spacing();

        ImGui::SetCursorPosX(
            (window_width - 260) * 0.5f);

        if (ImGui::Button(
                "Create Event",
                ImVec2(260, 50)))
        {
            printf("Create Event clicked\n");
        }

        ImGui::End();

        // Render
        ImGui::Render();

        int display_w, display_h;

        glfwGetFramebufferSize(
            window,
            &display_w,
            &display_h);

        glViewport(0, 0, display_w, display_h);

        glClearColor(
            1.0f,
            0.85f,
            0.90f,
            1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(
            ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
