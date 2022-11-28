//
// Created by ZoSand on 21/11/2022.
//

#include <stdexcept>
#include <map>
#include <set>
#include "Window.h"

[[maybe_unused]] void *Pepper::Shared::Window::GetWindow() const {
    if (m_handle == nullptr) {
        throw std::runtime_error("Handle not initialized");
    }
    return m_handle;
}

[[maybe_unused]] void Pepper::Shared::Window::SetWindow(void *_window) {
    m_handle = _window;
}

[[maybe_unused]] GLFWwindow *Pepper::Shared::Window::GetGlWindow() const {
    if (m_glWindow == nullptr) {
        throw std::runtime_error("GlHandle not initialized");
    }
    return m_glWindow;
}

Pepper::Shared::Window::Window()
        : m_handle(nullptr), m_glWindow(VK_NULL_HANDLE), m_vkInstance(VK_NULL_HANDLE), m_physicalDevice(VK_NULL_HANDLE),
          m_device(VK_NULL_HANDLE), m_graphicsQueue(VK_NULL_HANDLE), m_presentQueue(VK_NULL_HANDLE),
          m_surface(VK_NULL_HANDLE)
#if PEPPER_VULKAN_VALIDATE_LAYERS
        , m_vkValidationLayers()
#endif
{
    ::glfwInit();
    ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    ::glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    //TODO: set GLFW_DECORATED to GLFW_FALSE
    ::glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    //TODO: set GLFW_VISIBLE to GLFW_FALSE to start window as hidden
    ::glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
}

Pepper::Shared::Window::~Window() = default;

#   if PEPPER_VULKAN_VALIDATE_LAYERS

void Pepper::Shared::Window::InitValidationLayers() {
    ::uint32_t layerCount;
    std::vector<::VkLayerProperties> availableLayers(0);

    ::vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    availableLayers.resize(layerCount);

    ::vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    for (const char *layerName: m_vkValidationLayers) {
        bool layerFound = false;
        for (const auto &layerProperties: availableLayers) {
            if (::strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            throw std::runtime_error("validation layer not found");
        }
    }
    //TODO: add Message callback handling
}

#   endif

void Pepper::Shared::Window::InitInstanceInfos(::VkApplicationInfo *_appInfo, ::VkInstanceCreateInfo *_createInfo) {
    ::uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = ::glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

#   if PEPPER_VULKAN_VALIDATE_LAYERS // If we are in debug mode we enable validation layers for vulkan
    _createInfo->enabledLayerCount = static_cast<::uint32_t>(m_vkValidationLayers.size());
    _createInfo->ppEnabledLayerNames = m_vkValidationLayers.data();
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

void Pepper::Shared::Window::InitInstance() {
    ::VkApplicationInfo appInfo{};
    ::VkInstanceCreateInfo createInfo{};
    ::VkResult result;

    InitInstanceInfos(&appInfo, &createInfo);

    result = ::vkCreateInstance(&createInfo, nullptr, &m_vkInstance);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Instance");
    }
}

void Pepper::Shared::Window::CreateSurface() {
    ::VkResult result;

    result = ::glfwCreateWindowSurface(m_vkInstance, m_glWindow, nullptr, &m_surface);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface");
    }
}

::uint32_t Pepper::Shared::Window::RateDeviceSuitability(::VkPhysicalDevice _device) {
    ::VkPhysicalDeviceProperties deviceProperties;
    ::VkPhysicalDeviceFeatures deviceFeatures;
    ::uint32_t score = 0;

    ::vkGetPhysicalDeviceProperties(_device, &deviceProperties);
    ::vkGetPhysicalDeviceFeatures(_device, &deviceFeatures);

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }
    score += deviceProperties.limits.maxImageDimension2D;

    if (!deviceFeatures.geometryShader) {
        return 0;
    }
    return score;
}

void Pepper::Shared::Window::PickPhysicalDevice() {
    ::uint32_t deviceCount = 0;
    std::multimap<::uint32_t, ::VkPhysicalDevice> candidates;
    std::vector<::VkPhysicalDevice> devices(0);


    ::vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support");
    }
    devices.resize(deviceCount);

    ::vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, devices.data());
    for (const auto &device: devices) {
        ::uint32_t score = RateDeviceSuitability(device);
        candidates.insert(std::make_pair(score, device));
    }

    if (candidates.rbegin()->first > 0) {
        m_physicalDevice = candidates.rbegin()->second;
    } else {
        throw std::runtime_error("Failed to find a suitable GPU");
    }
}

Pepper::Shared::Window::QueueFamilyIndices Pepper::Shared::Window::FindQueueFamilies(::VkPhysicalDevice _device) {
    QueueFamilyIndices indices{};
    ::uint32_t queueFamilyCount = 0;
    ::VkBool32 presentsSupport = false;
    std::vector<::VkQueueFamilyProperties> queueFamilies;
    int i = 0;

    ::vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, nullptr);
    queueFamilies.resize(queueFamilyCount);

    ::vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, queueFamilies.data());
    for (const auto &queueFamily: queueFamilies) {
        ::vkGetPhysicalDeviceSurfaceSupportKHR(_device, i, m_surface, &presentsSupport);
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        if (presentsSupport) {
            indices.presentFamily = i;
        }
        if (indices.IsComplete()) {
            break;
        }
        i++;
    }

    return indices;
}

void Pepper::Shared::Window::InitDeviceInfos(QueueFamilyIndices _indices, ::VkDeviceCreateInfo *_deviceCreateInfo) {
    float queuePriority = 1.0f;
    ::VkPhysicalDeviceFeatures deviceFeatures{};
    std::vector<::VkDeviceQueueCreateInfo> queueCreateInfos{};
    std::set<::uint32_t> queueFamilies = {
            _indices.graphicsFamily.value(),
            _indices.presentFamily.value()
    };

    for (::uint32_t queueFamily: queueFamilies) {
        ::VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    _deviceCreateInfo->sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    _deviceCreateInfo->queueCreateInfoCount = static_cast<::uint32_t>(queueCreateInfos.size());
    _deviceCreateInfo->pQueueCreateInfos = queueCreateInfos.data();
    _deviceCreateInfo->pEnabledFeatures = &deviceFeatures;
    _deviceCreateInfo->enabledExtensionCount = 0;

#   if PEPPER_VULKAN_VALIDATE_LAYERS
    _deviceCreateInfo->enabledLayerCount = static_cast<::uint32_t>(m_vkValidationLayers.size());
    _deviceCreateInfo->ppEnabledLayerNames = m_vkValidationLayers.data();
#   else
    _deviceCreateInfo->enabledLayerCount = 0;
#   endif
}

void Pepper::Shared::Window::CreateLogicalDevice() {
    QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);
    ::VkDeviceCreateInfo deviceCreateInfo{};
    ::VkResult result;

    InitDeviceInfos(indices, &deviceCreateInfo);

    result = ::vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device");
    }

    ::vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
    ::vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
}

[[maybe_unused]] void Pepper::Shared::Window::Init(int _width, int _height) {
    m_glWindow = ::glfwCreateWindow(_width, _height, "ZWPWindow", nullptr, nullptr);
    if (m_glWindow == nullptr) {
        throw std::runtime_error("Failed to create GLFW window");
    }
#   if PEPPER_VULKAN_VALIDATE_LAYERS
    m_vkValidationLayers.emplace_back("VK_LAYER_KHRONOS_validation");
    InitValidationLayers();
#   endif
    InitInstance();
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();
}

[[maybe_unused]] void Pepper::Shared::Window::Update() {
    while (!::glfwWindowShouldClose(m_glWindow)) {
        ::glfwPollEvents();
    }
}

[[maybe_unused]] void Pepper::Shared::Window::Clear() {
    ::vkDestroyDevice(m_device, nullptr);
    ::vkDestroySurfaceKHR(m_vkInstance, m_surface, nullptr);
    ::vkDestroyInstance(m_vkInstance, nullptr);
    ::glfwDestroyWindow(m_glWindow);
    ::glfwTerminate();
}
