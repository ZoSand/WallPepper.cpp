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

//GLM dependencies
#   include <glm/vec2.hpp>
#   include <glm/vec3.hpp>

//other dependencies
#   include <array>
#   include <optional>
#   include <vector>

namespace Pepper::Core
{
	class PEPPER_SHARED_LIB VulkanEngine : public EngineBase
	{
	private:
#pragma region Constants
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
#pragma endregion

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

		struct Vertex
		{
			//TODO: switch to vec3
			glm::vec2 pos;
			glm::vec3 color;


			static ::VkVertexInputBindingDescription GetBindingDescription()
			{
				::VkVertexInputBindingDescription bindingDescription { };

				bindingDescription.binding = 0;
				bindingDescription.stride = sizeof(Vertex);
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				return bindingDescription;
			}

			static std::array<::VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
			{
				std::array<::VkVertexInputAttributeDescription, 2> attributeDescriptions { };

				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				//TODO: switch to vec3
				attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[0].offset = offsetof(Vertex, pos);

				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[1].offset = offsetof(Vertex, color);

				return attributeDescriptions;
			}
		};

#pragma endregion Structures Definitions

#pragma region Members
		::GLFWwindow* m_glWindow;
		::VkInstance m_vkInstance;
		::VkDebugUtilsMessengerEXT m_debugMessenger;
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

		::uint32_t m_currentFrame;

		bool m_framebufferResized;

		std::vector<::VkCommandBuffer> m_commandBuffers;
		std::vector<::VkSemaphore> m_imageAvailableSemaphores;
		std::vector<::VkSemaphore> m_renderFinishedSemaphores;
		std::vector<::VkFence> m_inFlightFences;

		std::vector<::VkImage> m_swapChainImages;
		std::vector<::VkImageView> m_swapChainImageViews;
		std::vector<const char*> m_deviceExtensions;
		std::vector<::VkFramebuffer> m_swapChainFramebuffers;

		std::vector<const char*> m_vkValidationLayers;

		std::vector<Vertex> m_vertices;

#pragma endregion Members

#pragma region Utils

		static VKAPI_ATTR ::VkBool32 VKAPI_CALL DebugCallback(
				::VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				::VkDebugUtilsMessageTypeFlagsEXT messageType,
				const ::VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
				void* pUserData);

		static void FramebufferResizeCallback(
				GLFWwindow* _window,
				int _width,
				int _height
		                                     );

		NO_DISCARD static bool CheckExtensionSupport(
				::VkPhysicalDevice _device,
				const std::vector<const char*> &_requiredExtensions
		                                            );

		NO_DISCARD static bool IsDeviceSuitable(
				::VkPhysicalDevice _device,
				const std::vector<const char*> &_requiredExtensions,
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
				::GLFWwindow* _window
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
				::VkPipelineShaderStageCreateInfo* _pipelineInfo,
				::VkShaderModule _shaderModule,
				::VkShaderStageFlagBits _stage
		                                );

		static void InitInstanceInfos(
				::VkApplicationInfo* _appInfo,
				::VkInstanceCreateInfo* _createInfo,
				const std::vector<const char*> &_validationLayers = { }
		                             );

		static void InitDeviceInfos(
				QueueFamilyIndices _indices,
				std::vector<::VkDeviceQueueCreateInfo>* _queueCreateInfos,
				::VkDeviceCreateInfo* _deviceCreateInfo,
				::VkPhysicalDeviceFeatures* deviceFeatures,
				const std::vector<const char*> &_deviceExtensions,
				const std::vector<const char*> &_validationLayers = { }
		                           );

		static void InitSwapChainInfos(
				const SwapChainSupportDetails &_swapChainSupport,
				::VkSwapchainCreateInfoKHR* _swapChainCreateInfo,
				::GLFWwindow* _glWindow,
				::VkPhysicalDevice _physicalDevice,
				::VkSurfaceKHR _surface,
				::VkExtent2D &_extent,
				::VkFormat &_imageFormat
		                              );

		static void InitImageViewInfos(
				::VkImageViewCreateInfo* _imageViewCreateInfo,
				::VkImage _swapChainImage,
				::VkFormat _swapChainImageFormat
		                              );

		static void InitRenderPassInfos(
				::VkAttachmentDescription* _colorAttachment,
				::VkAttachmentReference* _colorAttachmentRef,
				::VkSubpassDescription* _subpass,
				::VkRenderPassCreateInfo* _renderPassInfo,
				::VkFormat _swapChainImageFormat,
				::VkSubpassDependency* _subpassDependency
		                               );

#pragma endregion Info Initializers

#pragma region Utils

		static ::VkResult CreateDebugUtilsMessengerEXT(
				::VkInstance _instance,
				const ::VkDebugUtilsMessengerCreateInfoEXT* _pCreateInfo,
				const ::VkAllocationCallbacks* _pAllocator,
				::VkDebugUtilsMessengerEXT* _pDebugMessenger
		                                       );

		static void DestroyDebugUtilsMessengerEXT(
				::VkInstance _instance,
				::VkDebugUtilsMessengerEXT _debugMessenger,
				::VkAllocationCallbacks* _pAllocator
		                                  );

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

		static std::vector<const char*> GetRequiredExtensions();

		void InitValidationLayers();

		void InitInstance();

		void SetupDebugMessenger();

		void CreateSurface();

		void PickPhysicalDevice();

		void CreateLogicalDevice();

		void CreateSwapChain();

		void CleanupSwapChain();

		void RecreateSwapChain();

		void CreateImageViews();

		void CreateRenderPass();

		void CreateGraphicsPipeline();

		void CreateFramebuffers();

		void CreateCommandPool();

		void CreateCommandBuffers();

		void CreateSyncObjects();

#pragma endregion Instance Initializers

#pragma region Instance Cleaner

		void DestroyDebugMessenger();

#pragma endregion Instance Cleaner

	public:
		VulkanEngine();

		~VulkanEngine() override;

		void Init(
				int,
				int
		         ) override;

		void Update() override;

		void Shutdown() override;

		NO_DISCARD_UNUSED void* GetWindow() const override;

		NO_DISCARD bool ShouldClose() override;

		NO_DISCARD_UNUSED EngineType GetType() const override;
	};
}

#endif //PEPPER_SHARED_VULKAN_ENGINE_H
