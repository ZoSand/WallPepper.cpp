//
// Created by ZoSand on 21/11/2022.
//

#include <stdexcept>
#include "Window.h"

[[maybe_unused]] void *Pepper::Shared::Window::GetWindow() const
{
    if (m_handle == nullptr)
    {
        throw std::runtime_error("Handle not initialized");
    }
    return m_handle;
}

[[maybe_unused]] void Pepper::Shared::Window::SetWindow(void *_window)
{
    m_handle = _window;
}

[[maybe_unused]] GLFWwindow *Pepper::Shared::Window::GetGlWindow() const
{
    if (m_glWindow == nullptr)
    {
        throw std::runtime_error("GlHandle not initialized");
    }
    return m_glWindow;
}

Pepper::Shared::Window::Window()
        : m_handle(nullptr)
        , m_glWindow(nullptr)
        , m_vkInstance(nullptr)
{
    ::glfwInit();
    ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    ::glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    //TODO: set GLFW_DECORATED to GLFW_FALSE
    ::glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    InitInstance();
}

Pepper::Shared::Window::~Window() = default;

[[maybe_unused]] void Pepper::Shared::Window::Init(int _width, int _height)
{
    m_glWindow = ::glfwCreateWindow(_width, _height, "ZWPWindow", nullptr, nullptr);
}

[[maybe_unused]] void Pepper::Shared::Window::InitValidationLayers(::VkInstanceCreateInfo *_createInfo)
{
    std::vector<const char *> vkValidationLayers
            {
                    "VK_LAYER_KHRONOS_validation"
            };
    ::uint32_t layerCount;
    std::vector<::VkLayerProperties> availableLayers(0);

    ::vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    availableLayers.resize(layerCount);

    ::vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    for (const char *layerName: vkValidationLayers)
    {
        bool layerFound = false;
        for (const auto &layerProperties: availableLayers)
        {
            if (::strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }
        if (!layerFound)
        {
            throw std::runtime_error("validation layer not found");
        }
    }
    _createInfo->enabledLayerCount = static_cast<::uint32_t>(vkValidationLayers.size());
    _createInfo->ppEnabledLayerNames = vkValidationLayers.data();
    //TODO: add Message callback handling
}

void Pepper::Shared::Window::InitInstanceInfos(::VkApplicationInfo *_appInfo, ::VkInstanceCreateInfo *_createInfo)
{
    ::uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = ::glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

#   if PEPPER_VULKAN_VALIDATE_LAYERS // If we are in debug mode we enable validation layers for vulkan
    InitValidationLayers(_createInfo);
#   else
    _createInfo->enabledLayerCount = 0;
#   endif

    _appInfo->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    _appInfo->pApplicationName = "ZWP";
    _appInfo->applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    _appInfo->pEngineName = "No Engine";
    _appInfo->engineVersion = VK_MAKE_VERSION(1, 0, 0);
    _appInfo->apiVersion = VK_API_VERSION_1_0;

    _createInfo->sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    _createInfo->pApplicationInfo = _appInfo;
    _createInfo->enabledExtensionCount = glfwExtensionCount;
    _createInfo->ppEnabledExtensionNames = glfwExtensions;
    _createInfo->enabledLayerCount = 0;
}

void Pepper::Shared::Window::InitInstance()
{
    ::VkApplicationInfo appInfo{};
    ::VkInstanceCreateInfo createInfo{};
    ::VkResult result;

    InitInstanceInfos(&appInfo, &createInfo);

    result = ::vkCreateInstance(&createInfo, nullptr, &m_vkInstance);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Instance");
    }
}

[[maybe_unused]] void Pepper::Shared::Window::Update()
{
    while (!::glfwWindowShouldClose(m_glWindow))
    {
        ::glfwPollEvents();
    }
}

[[maybe_unused]] void Pepper::Shared::Window::Clear()
{
    ::vkDestroyInstance(m_vkInstance, nullptr);
    ::glfwDestroyWindow(m_glWindow);
    ::glfwTerminate();
}
