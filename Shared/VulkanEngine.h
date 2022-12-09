//
// Created by ZoSand on 29/11/2022.
//

#ifndef PEPPER_SHARED_VULKAN_ENGINE_H
#define PEPPER_SHARED_VULKAN_ENGINE_H

//this library internal dependencies
#   include "ConstantDefinitions.h" //should always be placed first
#   include "Library.h"
#   include "EngineBase.h"

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
    class PEPPER_SHARED_LIB VulkanEngine : public EngineBase
    {
    private:
#pragma region Members
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
        ::VkCommandPool m_commandPool;
        ::VkCommandBuffer m_commandBuffer;
        ::VkSemaphore m_imageAvailableSemaphore;
        ::VkSemaphore m_renderFinishedSemaphore;
        ::VkFence m_inFlightFence;

        std::vector<::VkImage> m_swapChainImages;
        std::vector<::VkImageView> m_swapChainImageViews;
        std::vector<const char *> m_deviceExtensions;
        std::vector<::VkFramebuffer> m_swapChainFramebuffers;

#   if PEPPER_VULKAN_VALIDATE_LAYERS

        std::vector<const char *> m_vkValidationLayers;

#   endif

#pragma endregion Members

#pragma region Structures Definitions

        struct QueueFamilyIndices
        {
            std::optional<::uint32_t> graphicsFamily;
            std::optional<::uint32_t> presentFamily;

            NO_DISCARD_UNUSED inline bool IsComplete() const
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

#pragma endregion Structures Definitions

#pragma region Utils

        NO_DISCARD static bool CheckExtensionSupport(
                ::VkPhysicalDevice _device,
                const std::vector<const char *> &_requiredExtensions
                                                    );

        NO_DISCARD static bool IsDeviceSuitable(
                ::VkPhysicalDevice _device,
                const std::vector<const char *> &_requiredExtensions,
                ::VkSurfaceKHR _surface
                                               );

        NO_DISCARD static ::uint32_t RateDeviceSuitability(
                ::VkPhysicalDevice _device
                                                          );

        NO_DISCARD static QueueFamilyIndices FindQueueFamilies(
                ::VkPhysicalDevice _device,
                ::VkSurfaceKHR _surface
                                                              );

        NO_DISCARD static SwapChainSupportDetails QuerySwapChainSupport(
                ::VkPhysicalDevice _device,
                ::VkSurfaceKHR _surface
                                                                       );

        NO_DISCARD static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
                const std::vector<VkSurfaceFormatKHR> &availableFormats
                                                                    );

        NO_DISCARD static ::VkPresentModeKHR ChooseSwapPresentMode(
                const std::vector<VkPresentModeKHR> &_availableModes
                                                                  );

        NO_DISCARD static ::VkExtent2D ChooseSwapExtent(
                ::VkSurfaceCapabilitiesKHR _capabilities,
                ::GLFWwindow *_window
                                                       );

        NO_DISCARD static std::vector<char> ReadShaderFile(
                const std::string &filename
                                                          );

        NO_DISCARD static ::VkShaderModule CreateShaderModule(
                const std::vector<char> &_code,
                ::VkDevice _device
                                                             );

#pragma endregion Utils

#pragma region Info Initializers

        static void InitShaderStageInfos(
                ::VkPipelineShaderStageCreateInfo *_pipelineInfo,
                ::VkShaderModule _shaderModule,
                ::VkShaderStageFlagBits _stage
                                        );

        static void InitInstanceInfos(
                ::VkApplicationInfo *_appInfo,
                ::VkInstanceCreateInfo *_createInfo,
                const std::vector<const char *> &_validationLayers = {}
                                     );

        static void InitDeviceInfos(
                QueueFamilyIndices _indices,
                std::vector<::VkDeviceQueueCreateInfo> *_queueCreateInfos,
                ::VkDeviceCreateInfo *_deviceCreateInfo,
                const std::vector<const char *> &_deviceExtensions,
                const std::vector<const char *> &_validationLayers = {}
                                   );

        static void InitSwapChainInfos(
                const SwapChainSupportDetails &_swapChainSupport,
                ::VkSwapchainCreateInfoKHR *_swapChainCreateInfo,
                ::GLFWwindow *_glWindow,
                ::VkPhysicalDevice _physicalDevice,
                ::VkSurfaceKHR _surface,
                ::VkExtent2D &_extent,
                ::VkFormat &_imageFormat
                                      );

        static void InitImageViewInfos(
                ::VkImageViewCreateInfo *_imageViewCreateInfo,
                ::VkImage _swapChainImage,
                ::VkFormat _swapChainImageFormat
                                      );

        static void InitRenderPassInfos(
                ::VkAttachmentDescription *_colorAttachment,
                ::VkAttachmentReference *_colorAttachmentRef,
                ::VkSubpassDescription *_subpass,
                ::VkRenderPassCreateInfo *_renderPassInfo,
                ::VkFormat _swapChainImageFormat,
                ::VkSubpassDependency* _subpassDependency
                                       );

#pragma endregion Info Initializers

#pragma region Utils

        static void RecordCommandBuffer(
                ::VkCommandBuffer _commandBuffer,
                ::VkRenderPass _renderPass,
                ::VkFramebuffer _framebuffer,
                ::VkExtent2D _swapChainExtent,
                ::VkPipeline _graphicsPipeline
                                );

        void DrawFrame();

#pragma endregion Utils

#pragma region Instance Initializers

#   if PEPPER_VULKAN_VALIDATE_LAYERS

        void InitValidationLayers();

#   endif

        void InitInstance();

        void CreateSurface();

        void PickPhysicalDevice();

        void CreateLogicalDevice();

        void CreateSwapChain();

        void CreateImageViews();

        void CreateRenderPass();

        void CreateGraphicsPipeline();

        void CreateFramebuffers();

        void CreateCommandPool();

        void CreateCommandBuffer();

        void CreateSyncObjects();

#pragma endregion Instance Initializers

    public:
        VulkanEngine();

        ~VulkanEngine() override;

        void Init(
                int,
                int
                 ) override;

        void Update() override;

        void Shutdown() override;

        NO_DISCARD_UNUSED void *GetWindow() const override;

        NO_DISCARD bool ShouldClose() override;

        NO_DISCARD_UNUSED EngineType GetType() const override;
    };
}

#endif //PEPPER_SHARED_VULKAN_ENGINE_H
