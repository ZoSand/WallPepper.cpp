//
// Created by ZoSand on 21/11/2022.
//

#include "Window.h"

[[maybe_unused]] void Pepper::Shared::Window::SetWindow(void *_window) {
    m_handle = _window;
}

Pepper::Shared::Window::Window()
        : m_handle(nullptr)
        , m_glWindow(nullptr) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
}

[[maybe_unused]] void * Pepper::Shared::Window::GetWindow() const {
    return m_handle;
}

[[maybe_unused]] void Pepper::Shared::Window::Init( int _width, int _height) {
    m_glWindow = glfwCreateWindow(_width, _height, "Vulkan Test Window", nullptr, nullptr);
}

[[maybe_unused]] GLFWwindow * Pepper::Shared::Window::GetGlWindow() const {
    return m_glWindow;
}

Pepper::Shared::Window::~Window() = default;
