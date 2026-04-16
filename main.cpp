#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <cstdio>
#include <OpenGL/gl3.h>   
#include <GLFW/glfw3.h>



int main()
{
   
    // 1. Initialize GLFW
    if (!glfwInit())
        return 1;

    // OpenGL version
const char* glsl_version = "#version 150";

// macOS OpenGL settings
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);



    // Create window
    GLFWwindow* window = glfwCreateWindow(
    1280,
    720,
    "Eventopia",
    NULL,
    NULL
);

if (window == NULL)
{
    printf("Window creation failed\n");
    glfwTerminate();
    return 1;
}

glfwMakeContextCurrent(window);
glfwSwapInterval(1);

    // 2. Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    // VERY IMPORTANT — initialization order
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 3. Render loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // -----------------------------
        // Welcome Page UI
        // -----------------------------

        ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin("Welcome to Eventopia");

        ImGui::Text("Welcome to Eventopia!");
        ImGui::Separator();

        ImGui::Text(
            "Eventopia helps you organize events like:\n"
            "- Weddings\n"
            "- Birthdays\n"
            "- Parties\n"
            "- Conferences"
        );

        ImGui::Spacing();

        ImGui::Text("Founders / Programmers:");
        ImGui::BulletText("Your Name");
        ImGui::BulletText("Team Member");

        ImGui::Spacing();

        if (ImGui::Button("Create Account / Login"))
        {
            printf("Login clicked\n");
        }

        if (ImGui::Button("Create Event"))
        {
            printf("Create Event clicked\n");
        }

        ImGui::End();

        // Rendering
        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(
            window,
            &display_w,
            &display_h
        );

        glViewport(0, 0, display_w, display_h);
glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(
            ImGui::GetDrawData()
        );

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
