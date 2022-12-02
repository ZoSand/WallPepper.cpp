//
// Created by ZoSand on 29/11/2022.
//

#include <stdexcept>
#include <limits>
#include <cstdint>
#include <algorithm>
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
            , m_swapChain(VK_NULL_HANDLE)
            , m_swapChainImageFormat(VK_FORMAT_UNDEFINED)
            , m_swapChainExtent({
                                        0,
                                        0
                                })
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
        RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get instance layer properties count")

        availableLayers.resize(layerCount);

        result = ::vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get instance layer properties")

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
            RUNTIME_ASSERT(layerFound, "Validation layer not found")
        }
        //TODO: add Message callback handling
    }

#   endif

    bool VulkanEngine::CheckExtensionSupport(::VkPhysicalDevice _device) const
    {
        ::uint32_t extensionCount;
        std::vector<::VkExtensionProperties> availableExtensions(0);
        std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());
        ::VkResult result;

        result = ::vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, nullptr);
        RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get device extension properties count")

        availableExtensions.resize(extensionCount);
        result = ::vkEnumerateDeviceExtensionProperties(_device, nullptr, &extensionCount, availableExtensions.data());
        RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get device extension properties")

        for (const auto &extension: availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    bool VulkanEngine::IsDeviceSuitable(::VkPhysicalDevice _device)
    {
        SwapChainSupportDetails swapChainSupport;
        bool extensionsSupported = CheckExtensionSupport(_device);
        bool swapChainAdequate = false;

        if (extensionsSupported)
        {
            swapChainSupport = QuerySwapChainSupport(_device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return extensionsSupported && swapChainAdequate;
    }

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
            RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to physical device surface support")

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
        RUNTIME_ASSERT(indices.IsComplete(), "Failed to find suitable queue families")

        return indices;
    }

    VulkanEngine::SwapChainSupportDetails VulkanEngine::QuerySwapChainSupport(::VkPhysicalDevice _device)
    {
        SwapChainSupportDetails details;
        ::uint32_t formatCount;
        ::uint32_t presentModeCount;
        ::VkResult result;

        result = ::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_device, m_surface, &details.capabilities);
        RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get physical device surface capabilities")

        result = ::vkGetPhysicalDeviceSurfaceFormatsKHR(_device, m_surface, &formatCount, nullptr);
        RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get physical device surface formats count")

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            result = ::vkGetPhysicalDeviceSurfaceFormatsKHR(_device, m_surface, &formatCount, details.formats.data());
            RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get physical device surface formats")
        }

        result = ::vkGetPhysicalDeviceSurfacePresentModesKHR(_device, m_surface, &presentModeCount, nullptr);
        RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get physical device surface present modes count")

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            result = ::vkGetPhysicalDeviceSurfacePresentModesKHR(_device, m_surface, &presentModeCount,
                                                                 details.presentModes.data());
            RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get physical device surface present modes")
        }

        return details;
    }

    VkSurfaceFormatKHR VulkanEngine::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &_availableFormats)
    {
        for (const auto &availableFormat: _availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
                    && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }
        return _availableFormats[0];
    }

    ::VkPresentModeKHR VulkanEngine::ChooseSwapPresentMode(std::vector<VkPresentModeKHR> _availableModes)
    {
        for (const auto &availableMode: _availableModes)
        {
            if (availableMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availableMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    ::VkExtent2D VulkanEngine::ChooseSwapExtent(::VkSurfaceCapabilitiesKHR _capabilities)
    {
        if (_capabilities.currentExtent.width != std::numeric_limits<::uint32_t>::max())
        {
            return _capabilities.currentExtent;
        }
        else
        {
            int width;
            int height;

            glfwGetFramebufferSize(m_glWindow, &width, &height);
            ::VkExtent2D actualExtent = {
                    static_cast<::uint32_t>(width),
                    static_cast<::uint32_t>(height)
            };
            actualExtent.width = std::clamp(actualExtent.width,
                                            _capabilities.minImageExtent.width,
                                            _capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height,
                                             _capabilities.minImageExtent.height,
                                             _capabilities.maxImageExtent.height);
            return actualExtent;
        }
    }

    void VulkanEngine::InitInstanceInfos(::VkApplicationInfo *_appInfo, ::VkInstanceCreateInfo *_createInfo)
    {
        ::uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = ::glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

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

#   if PEPPER_VULKAN_VALIDATE_LAYERS // If we are in debug mode we enable validation layers for vulkan
        _createInfo->enabledLayerCount = static_cast<::uint32_t>(m_vkValidationLayers.size());
        _createInfo->ppEnabledLayerNames = m_vkValidationLayers.data();
#   else //else we disable them
        _createInfo->enabledLayerCount = 0;
#   endif
    }

    void VulkanEngine::InitInstance()
    {
        ::VkApplicationInfo appInfo{};
        ::VkInstanceCreateInfo createInfo{};
        ::VkResult result;

        InitInstanceInfos(&appInfo, &createInfo);

        result = ::vkCreateInstance(&createInfo, nullptr, &m_vkInstance);
        RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create Instance")
    }

    void VulkanEngine::CreateSurface()
    {
        ::VkResult result;

        result = ::glfwCreateWindowSurface(m_vkInstance, m_glWindow, nullptr, &m_surface);
        RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create window surface")
    }

    void VulkanEngine::PickPhysicalDevice()
    {
        ::uint32_t deviceCount = 0;
        std::multimap<::uint32_t, ::VkPhysicalDevice> candidates;
        std::vector<::VkPhysicalDevice> devices(0);
        ::VkResult result;


        result = ::vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);
        RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get physical device count")
        RUNTIME_ASSERT(deviceCount != 0, "Failed to find GPUs with Vulkan support")

        devices.resize(deviceCount);

        result = ::vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, devices.data());
        RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get physical devices")

        for (const auto &device: devices)
        {
            ::uint32_t score = RateDeviceSuitability(device);
            candidates.insert(std::make_pair(score, device));
        }

        RUNTIME_ASSERT(candidates.rbegin()->first > 0, "Failed to find a suitable GPU")
        m_physicalDevice = candidates.rbegin()->second;
    }

    void VulkanEngine::InitDeviceInfos(
            QueueFamilyIndices _indices, std::vector<::VkDeviceQueueCreateInfo> *_queueCreateInfos,
            ::VkDeviceCreateInfo *_deviceCreateInfo
                                      )
    {
        float queuePriority = 1.0f;
        ::VkPhysicalDeviceFeatures deviceFeatures{};
        std::set<::uint32_t> queueFamilies = {
                _indices.graphicsFamily.value(),
                _indices.presentFamily.value()
        };

        for (::uint32_t queueFamily: queueFamilies)
        {
            ::VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            _queueCreateInfos->push_back(queueCreateInfo);
        }

        _deviceCreateInfo->sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        _deviceCreateInfo->queueCreateInfoCount = static_cast<::uint32_t>(_queueCreateInfos->size());
        _deviceCreateInfo->pQueueCreateInfos = _queueCreateInfos->data();
        _deviceCreateInfo->pEnabledFeatures = &deviceFeatures;
        _deviceCreateInfo->enabledExtensionCount = static_cast<::uint32_t>(m_deviceExtensions.size());
        _deviceCreateInfo->ppEnabledExtensionNames = m_deviceExtensions.data();

#   if PEPPER_VULKAN_VALIDATE_LAYERS
        _deviceCreateInfo->enabledLayerCount = static_cast<::uint32_t>(m_vkValidationLayers.size());
        _deviceCreateInfo->ppEnabledLayerNames = m_vkValidationLayers.data();
#   else
        _deviceCreateInfo->enabledLayerCount = 0;
#   endif
    }

    void VulkanEngine::InitSwapChainInfos(
            const VulkanEngine::SwapChainSupportDetails &_swapChainSupport,
            ::VkSwapchainCreateInfoKHR *_swapChainCreateInfo
                                         )
    {
        ::VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(_swapChainSupport.formats);
        ::VkPresentModeKHR presentMode = ChooseSwapPresentMode(_swapChainSupport.presentModes);
        ::VkExtent2D extent = ChooseSwapExtent(_swapChainSupport.capabilities);
        ::uint32_t imageCount = _swapChainSupport.capabilities.minImageCount + 1;
        QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);
        ::uint32_t queueFamilyIndices[] = {
                indices.graphicsFamily.value(),
                indices.presentFamily.value()
        };
        if (_swapChainSupport.capabilities.maxImageCount > 0 && imageCount > _swapChainSupport.capabilities
                                                                                              .maxImageCount)
        {
            imageCount = _swapChainSupport.capabilities.maxImageCount;
        }

        _swapChainCreateInfo->sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        _swapChainCreateInfo->surface = m_surface;
        _swapChainCreateInfo->minImageCount = imageCount;
        _swapChainCreateInfo->imageFormat = surfaceFormat.format;
        _swapChainCreateInfo->imageColorSpace = surfaceFormat.colorSpace;
        _swapChainCreateInfo->imageExtent = extent;
        _swapChainCreateInfo->imageArrayLayers = 1;
        _swapChainCreateInfo->imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        if (indices.graphicsFamily != indices.presentFamily)
        {
            _swapChainCreateInfo->imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            _swapChainCreateInfo->queueFamilyIndexCount = 2;
            _swapChainCreateInfo->pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            _swapChainCreateInfo->imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            _swapChainCreateInfo->queueFamilyIndexCount = 0;
            _swapChainCreateInfo->pQueueFamilyIndices = nullptr;
        }

        _swapChainCreateInfo->preTransform = _swapChainSupport.capabilities.currentTransform;
        _swapChainCreateInfo->compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        _swapChainCreateInfo->presentMode = presentMode;
        _swapChainCreateInfo->clipped = VK_TRUE;
        _swapChainCreateInfo->oldSwapchain = VK_NULL_HANDLE;

        m_swapChainExtent = extent;
        m_swapChainImageFormat = surfaceFormat.format;
    }

    void VulkanEngine::CreateLogicalDevice()
    {
        QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);
        ::VkDeviceCreateInfo deviceCreateInfo{};
        ::VkResult result;
        std::vector<::VkDeviceQueueCreateInfo> queueCreateInfos;

        RUNTIME_ASSERT(indices.IsComplete(), "Failed to find suitable queue families")
        RUNTIME_ASSERT(IsDeviceSuitable(m_physicalDevice), "Failed to find a suitable GPU")

        InitDeviceInfos(indices, &queueCreateInfos, &deviceCreateInfo);

        result = ::vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device);
        RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create logical device")

        ::vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
        ::vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
    }

    void VulkanEngine::CreateSwapChain()
    {
        SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_physicalDevice);
        ::VkSwapchainCreateInfoKHR createInfo{};
        ::uint32_t swapChainImageCount;
        ::VkResult result;

        InitSwapChainInfos(swapChainSupport, &createInfo);

        result = ::vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain);
        RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create swap chain")

        result = ::vkGetSwapchainImagesKHR(m_device, m_swapChain, &swapChainImageCount, nullptr);
        RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get swap chain images count")

        m_swapChainImages.resize(swapChainImageCount);
        result = ::vkGetSwapchainImagesKHR(m_device, m_swapChain, &swapChainImageCount, m_swapChainImages.data());
        RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get swap chain images")
    }

    void VulkanEngine::Init(int _width, int _height)
    {
        int result;

        result = ::glfwInit();
        RUNTIME_ASSERT(result == GLFW_TRUE, "Failed to initialize GLFW")

        ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        ::glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        //TODO: set GLFW_DECORATED to GLFW_FALSE
        ::glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        //TODO: set GLFW_VISIBLE to GLFW_FALSE to start window as hidden
        ::glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

        m_glWindow = ::glfwCreateWindow(_width, _height, "ZWP", nullptr, nullptr);
        RUNTIME_ASSERT(m_glWindow != nullptr, "Failed to create GLFW window")

        m_deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

#   if PEPPER_VULKAN_VALIDATE_LAYERS
        m_vkValidationLayers.emplace_back("VK_LAYER_KHRONOS_validation");
        InitValidationLayers();
#   endif

        InitInstance();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
    }

    void VulkanEngine::Update()
    {
        ::glfwPollEvents();
    }

    void VulkanEngine::Shutdown()
    {
        ::vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
        ::vkDestroyDevice(m_device, nullptr);
        ::vkDestroySurfaceKHR(m_vkInstance, m_surface, nullptr);
        ::vkDestroyInstance(m_vkInstance, nullptr);
        ::glfwDestroyWindow(m_glWindow);
        ::glfwTerminate();
    }

    void *VulkanEngine::GetWindow() const
    {
        RUNTIME_ASSERT(m_glWindow != nullptr, "Failed to get GLFW window")
        return static_cast<void *>(m_glWindow);
    }

    bool VulkanEngine::ShouldClose()
    {
        RUNTIME_ASSERT(m_glWindow != nullptr, "Failed to get GLFW window")
        return ::glfwWindowShouldClose(m_glWindow);
    }

    IEngine::EngineType VulkanEngine::GetType() const
    {
        RUNTIME_ASSERT(m_glWindow != nullptr, "Engine is not initialized")
        return IEngine::EngineType::Vulkan;
    }
}