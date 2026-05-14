#!/bin/bash

# Build script for myapp with Calendar UI
# This script compiles the C++ ImGui application with the new weekly calendar view

echo "Building myapp with Weekly Calendar UI..."
echo "==========================================="

# Compile the application with correct include paths
clang++ -std=c++17 \
    -I./imgui \
    -I./imgui/backends \
    -I/opt/homebrew/include \
    -L/opt/homebrew/lib \
    main.cpp \
    imgui/imgui.cpp \
    imgui/imgui_draw.cpp \
    imgui/imgui_tables.cpp \
    imgui/imgui_widgets.cpp \
    imgui/backends/imgui_impl_glfw.cpp \
    imgui/backends/imgui_impl_opengl3.cpp \
    -lglfw \
    -framework OpenGL \
    -o myapp

if [ $? -eq 0 ]; then
    echo ""
    echo "✓ Build successful! Binary: ./myapp"
    echo ""
    echo "To run the application:"
    echo "  ./myapp"
    echo ""
else
    echo ""
    echo "✗ Build failed"
    echo ""
    echo "Troubleshooting:"
    echo "1. Ensure Xcode Command Line Tools are installed:"
    echo "   xcode-select --install"
    echo ""
    echo "2. Ensure GLFW is installed:"
    echo "   brew install glfw"
    echo ""
    exit 1
fi


