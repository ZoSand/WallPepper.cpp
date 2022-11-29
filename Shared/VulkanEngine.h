//
// Created by ZoSand on 29/11/2022.
//

#ifndef PEPPER_SHARED_VULKAN_ENGINE_H
#define PEPPER_SHARED_VULKAN_ENGINE_H

//this library internal dependencies
#   include "ConstantDefinitions.h" //should always be placed first
#   include "Library.h"
#   include "IEngine.h"

//GLFW dependencies
#   define GLFW_INCLUDE_VULKAN
#   define GLFW_EXPOSE_NATIVE_WIN32

#   include <glfw/glfw3.h>
#   include <GLFW/glfw3native.h>

//other dependencies
#   include <vector>
#   include <optional>

namespace Pepper::Core
{
    class PEPPER_SHARED_LIB VulkanEngine : public IEngine
    {
    private:
        ::GLFWwindow *m_glWindow;
        ::VkInstance m_vkInstance;
        ::VkPhysicalDevice m_physicalDevice;
        ::VkDevice m_device;
        ::VkQueue m_graphicsQueue;
        ::VkQueue m_presentQueue;
        ::VkSurfaceKHR m_surface;

#       if PEPPER_VULKAN_VALIDATE_LAYERS

        std::vector<const char *> m_vkValidationLayers;

        void InitValidationLayers();

#       endif

        struct QueueFamilyIndices
        {
            std::optional<::uint32_t> graphicsFamily;
            std::optional<::uint32_t> presentFamily;

            NO_DISCARD_UNUSED bool IsComplete() const
            {
                return graphicsFamily.has_value() && presentFamily.has_value();
            }
        };

        NO_DISCARD static ::uint32_t RateDeviceSuitability(::VkPhysicalDevice _device);

        NO_DISCARD QueueFamilyIndices FindQueueFamilies(::VkPhysicalDevice _device);

        void InitInstanceInfos(::VkApplicationInfo *_appInfo, ::VkInstanceCreateInfo *_createInfo);

        void InitDeviceInfos(
                QueueFamilyIndices _indices, std::vector<::VkDeviceQueueCreateInfo> *_queueCreateInfos,
                ::VkDeviceCreateInfo *_deviceCreateInfo
                            );

        void InitInstance();

        void CreateSurface();

        void PickPhysicalDevice();

        void CreateLogicalDevice();

    public:
        VulkanEngine();

        ~VulkanEngine() override;

        NO_DISCARD_UNUSED void *GetWindow() const override;

        NO_DISCARD_UNUSED EngineType GetType() const override;

        NO_DISCARD bool ShouldClose() override;

        void Init(int, int) override;

        void Update() override;

        void Shutdown() override;

    };
}

#endif //PEPPER_SHARED_VULKAN_ENGINE_H
