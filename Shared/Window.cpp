//
// Created by ZoSand on 21/11/2022.
//

#include <stdexcept>
#include "Window.h"

[[maybe_unused]] void Pepper::Shared::Window::SetWindow(void *_window) {
    m_handle = _window;
}

Pepper::Shared::Window::Window()
        : m_handle(nullptr), m_glWindow(nullptr) {
    ::glfwInit();
    ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    ::glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    ::glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    InitInstance();
}

[[maybe_unused]] void *Pepper::Shared::Window::GetWindow() const {
    if (m_handle == nullptr) {
        throw std::runtime_error("Handle not initialized");
    }
    return m_handle;
}

[[maybe_unused]] void Pepper::Shared::Window::Init(int _width, int _height) {
    m_glWindow = ::glfwCreateWindow(_width, _height, "ZWP", nullptr, nullptr);
}

[[maybe_unused]] GLFWwindow *Pepper::Shared::Window::GetGlWindow() const {
    if (m_glWindow == nullptr) {
        throw std::runtime_error("GlHandle not initialized");
    }
    return m_glWindow;
}

Pepper::Shared::Window::~Window() {
    ::vkDestroyInstance(m_vkInstance, nullptr);
    ::glfwDestroyWindow(m_glWindow);
    ::glfwTerminate();
}

[[maybe_unused]] void Pepper::Shared::Window::Update() {
    ::glfwPollEvents();
}

void Pepper::Shared::Window::InitInstance() {
    ::VkApplicationInfo appInfo{};
    ::VkInstanceCreateInfo createInfo{};
    ::VkResult result = VK_SUCCESS;
    ::uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;

    glfwExtensions = ::glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "ZWP";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount = 0;

    result = ::vkCreateInstance(&createInfo, nullptr, &m_vkInstance);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Instance");
    }
}
