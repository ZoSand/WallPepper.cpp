//
// Created by ZoSand on 29/11/2022.
//

#include <stdexcept>
#include <map>
#include <set>

#include "VulkanEngine.h"

namespace Pepper::Core
{
    VulkanEngine::VulkanEngine()
            : m_glWindow(VK_NULL_HANDLE)
            , m_vkInstance(VK_NULL_HANDLE)
            , m_physicalDevice(VK_NULL_HANDLE)
            , m_device(VK_NULL_HANDLE)
            , m_graphicsQueue(VK_NULL_HANDLE)
            , m_presentQueue(VK_NULL_HANDLE)
            , m_surface(VK_NULL_HANDLE)
#   if PEPPER_VULKAN_VALIDATE_LAYERS
            , m_vkValidationLayers()
#   endif
    {}

    VulkanEngine::~VulkanEngine() = default;

#   if PEPPER_VULKAN_VALIDATE_LAYERS

    void VulkanEngine::InitValidationLayers()
    {
        ::uint32_t layerCount;
        std::vector<::VkLayerProperties> availableLayers(0);
        ::VkResult result;

        result = ::vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to get instance layer properties count");
        }
        availableLayers.resize(layerCount);

        result = ::vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to get instance layer properties");
        }

        for (const char *layerName: m_vkValidationLayers)
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
        //TODO: add Message callback handling
    }

#   endif

    ::uint32_t VulkanEngine::RateDeviceSuitability(::VkPhysicalDevice _device)
    {
        ::VkPhysicalDeviceProperties deviceProperties;
        ::VkPhysicalDeviceFeatures deviceFeatures;
        ::uint32_t score = 0;

        ::vkGetPhysicalDeviceProperties(_device, &deviceProperties);
        ::vkGetPhysicalDeviceFeatures(_device, &deviceFeatures);

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            score += 1000;
        }
        score += deviceProperties.limits.maxImageDimension2D;

        if (!deviceFeatures.geometryShader)
        {
            return 0;
        }
        return score;
    }

    VulkanEngine::QueueFamilyIndices VulkanEngine::FindQueueFamilies(::VkPhysicalDevice _device)
    {
        QueueFamilyIndices indices{};
        ::uint32_t queueFamilyCount = 0;
        ::VkBool32 presentsSupport = false;
        std::vector<::VkQueueFamilyProperties> queueFamilies;
        int i = 0;

        ::vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, nullptr);
        queueFamilies.resize(queueFamilyCount);

        ::vkGetPhysicalDeviceQueueFamilyProperties(_device, &queueFamilyCount, queueFamilies.data());
        for (const auto &queueFamily: queueFamilies)
        {
            ::VkResult result;
            result = ::vkGetPhysicalDeviceSurfaceSupportKHR(_device, i, m_surface, &presentsSupport);
            if (result != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to get physical device surface support");
            }
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }
            if (presentsSupport)
            {
                indices.presentFamily = i;
            }
            if (indices.IsComplete())
            {
                break;
            }
            i++;
        }

        if (!indices.IsComplete())
        {
            throw std::runtime_error("failed to find suitable queue families");
        }
        return indices;
    }

    void VulkanEngine::InitInstanceInfos(::VkApplicationInfo *_appInfo, ::VkInstanceCreateInfo *_createInfo)
    {
        ::uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = ::glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

#   if PEPPER_VULKAN_VALIDATE_LAYERS // If we are in debug mode we enable validation layers for vulkan
        _createInfo->enabledLayerCount = static_cast<::uint32_t>(m_vkValidationLayers.size());
        _createInfo->ppEnabledLayerNames = m_vkValidationLayers.data();
#   else //else we disable them
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

    void VulkanEngine::InitInstance()
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

    void VulkanEngine::CreateSurface()
    {
        ::VkResult result;

        result = ::glfwCreateWindowSurface(m_vkInstance, m_glWindow, nullptr, &m_surface);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create window surface");
        }
    }

    void VulkanEngine::PickPhysicalDevice()
    {
        ::uint32_t deviceCount = 0;
        std::multimap<::uint32_t, ::VkPhysicalDevice> candidates;
        std::vector<::VkPhysicalDevice> devices(0);
        ::VkResult result;


        result = ::vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to enumerate physical devices");
        }
        if (deviceCount == 0)
        {
            throw std::runtime_error("Failed to find GPUs with Vulkan support");
        }
        devices.resize(deviceCount);

        result = ::vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, devices.data());
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to enumerate physical devices");
        }
        for (const auto &device: devices)
        {
            ::uint32_t score = RateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
        }

        if (candidates.rbegin()->first > 0)
        {
            m_physicalDevice = candidates.rbegin()->second;
        }
        else
        {
            throw std::runtime_error("Failed to find a suitable GPU");
        }
    }

    void VulkanEngine::InitDeviceInfos(QueueFamilyIndices _indices, ::VkDeviceCreateInfo *_deviceCreateInfo)
    {
        float queuePriority = 1.0f;
        ::VkPhysicalDeviceFeatures deviceFeatures{};
        std::set<::uint32_t> queueFamilies = {
                _indices.graphicsFamily.value(),
                _indices.presentFamily.value()
        };
        std::vector<::VkDeviceQueueCreateInfo> queueCreateInfos(queueFamilies.size());

        for (::uint32_t queueFamily: queueFamilies)
        {
            ::VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.emplace_back(queueCreateInfo);
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

    void VulkanEngine::CreateLogicalDevice()
    {
        QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);
        ::VkDeviceCreateInfo deviceCreateInfo{};
        ::VkResult result;

        InitDeviceInfos(indices, &deviceCreateInfo);

        result = ::vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create logical device");
        }

        ::vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
        ::vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
    }

    void VulkanEngine::Init(int _width, int _height)
    {
        int result;

        result = ::glfwInit();
        if (result == GLFW_FALSE)
        {
            throw std::runtime_error("Failed to init GLFW");
        }
        ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        ::glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        //TODO: set GLFW_DECORATED to GLFW_FALSE
        ::glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        //TODO: set GLFW_VISIBLE to GLFW_FALSE to start window as hidden
        ::glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

        m_glWindow = ::glfwCreateWindow(_width, _height, "ZWP", nullptr, nullptr);
        if (m_glWindow == nullptr)
        {
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

    void VulkanEngine::Update()
    {
        ::glfwPollEvents();
    }

    void VulkanEngine::Shutdown()
    {
        ::vkDestroyDevice(m_device, nullptr);
        ::vkDestroySurfaceKHR(m_vkInstance, m_surface, nullptr);
        ::vkDestroyInstance(m_vkInstance, nullptr);
        ::glfwDestroyWindow(m_glWindow);
        ::glfwTerminate();
    }

    void *VulkanEngine::GetWindow() const
    {
        if (m_glWindow == nullptr)
        {
            throw std::runtime_error("GlHandle not initialized");
        }
        return static_cast<void *>(m_glWindow);
    }

    bool VulkanEngine::ShouldClose()
    {
        return ::glfwWindowShouldClose(m_glWindow);
    }
}