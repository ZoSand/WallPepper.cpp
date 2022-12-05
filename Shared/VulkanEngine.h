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
        ::VkSwapchainKHR m_swapChain;
        ::VkFormat m_swapChainImageFormat;
        ::VkExtent2D m_swapChainExtent;
        ::VkRenderPass m_renderPass;
        ::VkPipelineLayout m_pipelineLayout;
        ::VkPipeline m_graphicsPipeline;

        std::vector<::VkImage> m_swapChainImages;
        std::vector<::VkImageView> m_swapChainImageViews;
        std::vector<const char *> m_deviceExtensions;

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

        struct SwapChainSupportDetails
        {
            ::VkSurfaceCapabilitiesKHR capabilities;
            std::vector<::VkSurfaceFormatKHR> formats;
            std::vector<::VkPresentModeKHR> presentModes;
        };

        NO_DISCARD bool CheckExtensionSupport(::VkPhysicalDevice device) const;

        NO_DISCARD bool IsDeviceSuitable(::VkPhysicalDevice _device);

        NO_DISCARD static ::uint32_t RateDeviceSuitability(::VkPhysicalDevice _device);

        NO_DISCARD QueueFamilyIndices FindQueueFamilies(::VkPhysicalDevice _device);

        NO_DISCARD SwapChainSupportDetails QuerySwapChainSupport(::VkPhysicalDevice _device);

        NO_DISCARD static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
                const std::vector<VkSurfaceFormatKHR>
                &availableFormats
                                                                    );

        NO_DISCARD static ::VkPresentModeKHR ChooseSwapPresentMode(
                const std::vector<VkPresentModeKHR> &
                _availableModes
                                                                  );

        NO_DISCARD ::VkExtent2D ChooseSwapExtent(::VkSurfaceCapabilitiesKHR _capabilities);

        void InitInstanceInfos(::VkApplicationInfo *_appInfo, ::VkInstanceCreateInfo *_createInfo);

        void InitDeviceInfos(
                QueueFamilyIndices _indices, std::vector<::VkDeviceQueueCreateInfo> *_queueCreateInfos,
                ::VkDeviceCreateInfo *_deviceCreateInfo
                            );

        void InitSwapChainInfos(
                const VulkanEngine::SwapChainSupportDetails &_swapChainSupport,
                ::VkSwapchainCreateInfoKHR *_swapChainCreateInfo
                               );

        void InitImageViewInfos(
                ::VkImageViewCreateInfo *_imageViewCreateInfo,
                ::VkImage _swapChainImage
                               );

        static std::vector<char> ReadShaderFile(const std::string &filename);

        static void InitShaderStageInfos(::VkPipelineShaderStageCreateInfo* _pipelineInfo, ::VkShaderModule _shaderModule, ::VkShaderStageFlagBits _stage);

        static ::VkShaderModule CreateShaderModule(const std::vector<char> &_code, ::VkDevice _device);

        void InitInstance();

        void CreateSurface();

        void PickPhysicalDevice();

        void CreateLogicalDevice();

        void CreateSwapChain();

        void CreateImageViews();

        void CreateRenderPass();

        void CreateGraphicsPipeline();

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
