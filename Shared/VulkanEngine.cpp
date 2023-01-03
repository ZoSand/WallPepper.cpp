//
// Created by ZoSand on 29/11/2022.
//

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>

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
			, m_swapChainImageFormat(::VkFormat::VK_FORMAT_UNDEFINED)
			, m_swapChainExtent({
					                    0,
					                    0
			                    })
			, m_renderPass(VK_NULL_HANDLE)
			, m_pipelineLayout(VK_NULL_HANDLE)
			, m_graphicsPipeline(VK_NULL_HANDLE)
			, m_commandPool(VK_NULL_HANDLE)
			, m_currentFrame(0)
			, m_framebufferResized(false)
			, m_commandBuffers(MAX_FRAMES_IN_FLIGHT)
			, m_imageAvailableSemaphores(MAX_FRAMES_IN_FLIGHT)
			, m_renderFinishedSemaphores(MAX_FRAMES_IN_FLIGHT)
			, m_inFlightFences(MAX_FRAMES_IN_FLIGHT)
			, m_deviceExtensions({
					                     VK_KHR_SWAPCHAIN_EXTENSION_NAME
			                     })
			, m_swapChainImages()
			, m_swapChainImageViews()
			, m_swapChainFramebuffers()
			, m_vertices({
					             {{ 0.0f,  -0.5f }, { 1.0f, 0.0f, 0.0f }},
					             {{ 0.5f,  0.5f },  { 0.0f, 1.0f, 0.0f }},
					             {{ -0.5f, 0.5f },  { 0.0f, 0.0f, 1.0f }}
			             })
#   if PEPPER_VULKAN_VALIDATE_LAYERS
			, m_vkValidationLayers({
					                       "VK_LAYER_KHRONOS_validation"
			                       })
#   else
	, m_vkValidationLayers()
#   endif
	{}

	VulkanEngine::~VulkanEngine() = default;

	std::vector<const char*> VulkanEngine::GetRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		std::vector<const char*> extensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		extensions.reserve(glfwExtensionCount + 1);
		for (uint32_t i = 0; i < glfwExtensionCount; ++i)
		{
			extensions.push_back(glfwExtensions[i]);
		}

#   if PEPPER_VULKAN_VALIDATE_LAYERS
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#   endif

		return extensions;
	}

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

		for (const char* layerName: m_vkValidationLayers)
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

	::VkBool32 VulkanEngine::DebugCallback(
			::VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			::VkDebugUtilsMessageTypeFlagsEXT messageType,
			const ::VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData
	                                      )
	{
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		}

		return VK_FALSE;
	}

	void VulkanEngine::FramebufferResizeCallback(
			GLFWwindow* _window,
			int _width,
			int _height
	                                            )
	{
		//remove warning about unused parameters
		(void) _width;
		(void) _height;

		auto engine = reinterpret_cast<VulkanEngine*>(::glfwGetWindowUserPointer(_window));
		engine->m_framebufferResized = true;
	}

	bool VulkanEngine::CheckExtensionSupport(
			::VkPhysicalDevice _device,
			const std::vector<const char*> &_requiredExtensions
	                                        )
	{
		::uint32_t extensionCount;
		std::vector<::VkExtensionProperties> availableExtensions(0);
		std::set<std::string> requiredExtensions(_requiredExtensions.begin(), _requiredExtensions.end());
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

	bool VulkanEngine::IsDeviceSuitable(
			::VkPhysicalDevice _device,
			const std::vector<const char*> &_requiredExtensions,
			::VkSurfaceKHR _surface
	                                   )
	{
		SwapChainSupportDetails swapChainSupport;
		bool extensionsSupported = CheckExtensionSupport(_device, _requiredExtensions);
		bool swapChainAdequate;

		if (extensionsSupported)
		{
			swapChainSupport = QuerySwapChainSupport(_device, _surface);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		return extensionsSupported && swapChainAdequate;
	}

	::uint32_t VulkanEngine::RateDeviceSuitability(
			::VkPhysicalDevice _device
	                                              )
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

	VulkanEngine::QueueFamilyIndices VulkanEngine::FindQueueFamilies(
			::VkPhysicalDevice _device,
			::VkSurfaceKHR _surface
	                                                                )
	{
		QueueFamilyIndices indices { };
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
			result = ::vkGetPhysicalDeviceSurfaceSupportKHR(_device, i, _surface, &presentsSupport);
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

	VulkanEngine::SwapChainSupportDetails VulkanEngine::QuerySwapChainSupport(
			::VkPhysicalDevice _device,
			::VkSurfaceKHR _surface
	                                                                         )
	{
		SwapChainSupportDetails details;
		::uint32_t formatCount;
		::uint32_t presentModeCount;
		::VkResult result;

		result = ::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_device, _surface, &details.capabilities);
		RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get physical device surface capabilities")

		result = ::vkGetPhysicalDeviceSurfaceFormatsKHR(_device, _surface, &formatCount, nullptr);
		RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get physical device surface formats count")

		if (formatCount != 0)
		{
			details.formats
			       .resize(formatCount);
			result = ::vkGetPhysicalDeviceSurfaceFormatsKHR(_device, _surface, &formatCount, details.formats.data());
			RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get physical device surface formats")
		}

		result = ::vkGetPhysicalDeviceSurfacePresentModesKHR(_device, _surface,
		                                                     &presentModeCount, nullptr);
		RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get physical device surface present modes count")

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			result = ::vkGetPhysicalDeviceSurfacePresentModesKHR(_device, _surface, &presentModeCount,
			                                                     details.presentModes.data());
			RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get physical device surface present modes")
		}

		return details;
	}

	VkSurfaceFormatKHR VulkanEngine::ChooseSwapSurfaceFormat(
			const std::vector<VkSurfaceFormatKHR> &_availableFormats
	                                                        )
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

	::VkPresentModeKHR VulkanEngine::ChooseSwapPresentMode(
			const std::vector<VkPresentModeKHR> &_availableModes
	                                                      )
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

	::VkExtent2D VulkanEngine::ChooseSwapExtent(
			::VkSurfaceCapabilitiesKHR _capabilities,
			::GLFWwindow* _window
	                                           )
	{
		if (_capabilities.currentExtent.width != std::numeric_limits<::uint32_t>::max())
		{
			return _capabilities.currentExtent;
		}
		else
		{
			int width;
			int height;

			glfwGetFramebufferSize(_window, &width, &height);
			::VkExtent2D actualExtent = {
					static_cast<::uint32_t>(width),
					static_cast<::uint32_t>(height)
			};
			actualExtent.width = std::clamp(actualExtent.width, _capabilities.minImageExtent.width,
			                                _capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, _capabilities.minImageExtent.height,
			                                 _capabilities.maxImageExtent.height);
			return actualExtent;
		}
	}

	std::vector<char> VulkanEngine::ReadShaderFile(
			const std::string &filename
	                                              )
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		long long fileSize;
		std::vector<char> buffer(0);
		RUNTIME_ASSERT(file.is_open(), "Failed to open shader file")

		fileSize = static_cast<long long>(file.tellg());
		buffer.resize(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

	::VkShaderModule VulkanEngine::CreateShaderModule(
			const std::vector<char> &_code,
			::VkDevice _device
	                                                 )
	{
		::VkShaderModuleCreateInfo createInfo = { };
		::VkShaderModule shaderModule;
		::VkResult result;

		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = _code.size();
		createInfo.pCode = reinterpret_cast<const ::uint32_t*>(_code.data());

		result = ::vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule);
		RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create shader module")

		return shaderModule;
	}

	void VulkanEngine::InitShaderStageInfos(
			::VkPipelineShaderStageCreateInfo* _pipelineInfo,
			::VkShaderModule _shaderModule,
			::VkShaderStageFlagBits _stage
	                                       )
	{
		_pipelineInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		_pipelineInfo->stage = _stage;
		_pipelineInfo->module = _shaderModule;
		_pipelineInfo->pName = "main";
	}

	void VulkanEngine::InitInstanceInfos(
			::VkApplicationInfo* _appInfo,
			::VkInstanceCreateInfo* _createInfo,
			const std::vector<const char*> &_validationLayers
	                                    )
	{
		std::vector<const char*> extensions = GetRequiredExtensions();

		_appInfo->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		_appInfo->pApplicationName = "ZWP";
		_appInfo->applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		_appInfo->pEngineName = "No Engine";
		_appInfo->engineVersion = VK_MAKE_VERSION(1, 0, 0);
		_appInfo->apiVersion = VK_API_VERSION_1_0;

		_createInfo->sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		_createInfo->pApplicationInfo = _appInfo;
		_createInfo->enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		_createInfo->ppEnabledExtensionNames = extensions.data();
		_createInfo->enabledLayerCount = static_cast<::uint32_t>(_validationLayers.size());
		_createInfo->ppEnabledLayerNames = _validationLayers.data();
	}

	void VulkanEngine::InitDeviceInfos(
			QueueFamilyIndices _indices,
			std::vector<::VkDeviceQueueCreateInfo>* _queueCreateInfos,
			::VkDeviceCreateInfo* _deviceCreateInfo,
			::VkPhysicalDeviceFeatures* _deviceFeatures,
			const std::vector<const char*> &_deviceExtensions,
			const std::vector<const char*> &_validationLayers
	                                  )
	{
		float queuePriority = 1.0f;
		std::set<::uint32_t> queueFamilies = {
				_indices.graphicsFamily.value(),
				_indices.presentFamily.value()
		};

		for (::uint32_t queueFamily: queueFamilies)
		{
			::VkDeviceQueueCreateInfo queueCreateInfo { };
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			_queueCreateInfos->push_back(queueCreateInfo);
		}

		_deviceCreateInfo->sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		_deviceCreateInfo->queueCreateInfoCount = static_cast<::uint32_t>(_queueCreateInfos->size());
		_deviceCreateInfo->pQueueCreateInfos = _queueCreateInfos->data();
		_deviceCreateInfo->pEnabledFeatures = _deviceFeatures;
		_deviceCreateInfo->enabledExtensionCount = static_cast<::uint32_t>(_deviceExtensions.size());
		_deviceCreateInfo->ppEnabledExtensionNames = _deviceExtensions.data();
		_deviceCreateInfo->enabledLayerCount = static_cast<::uint32_t>(_validationLayers.size());
		_deviceCreateInfo->ppEnabledLayerNames = _validationLayers.data();
	}

	void VulkanEngine::InitSwapChainInfos(
			const VulkanEngine::SwapChainSupportDetails &_swapChainSupport,
			::VkSwapchainCreateInfoKHR* _swapChainCreateInfo,
			::GLFWwindow* _glWindow,
			::VkPhysicalDevice _physicalDevice,
			::VkSurfaceKHR _surface,
			::VkExtent2D &_extent,
			::VkFormat &_imageFormat
	                                     )
	{
		::VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(_swapChainSupport.formats);
		::VkPresentModeKHR presentMode = ChooseSwapPresentMode(_swapChainSupport.presentModes);
		::VkExtent2D extent = ChooseSwapExtent(_swapChainSupport.capabilities, _glWindow);
		::uint32_t imageCount = _swapChainSupport.capabilities.minImageCount + 1;
		QueueFamilyIndices indices = FindQueueFamilies(_physicalDevice, _surface);
		::uint32_t queueFamilyIndices[] = {
				indices.graphicsFamily.value(),
				indices.presentFamily.value()
		};
		if (_swapChainSupport.capabilities.maxImageCount > 0
				&& imageCount > _swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = _swapChainSupport.capabilities.maxImageCount;
		}

		//TODO: move to a separate function
		_swapChainCreateInfo->sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		_swapChainCreateInfo->surface = _surface;
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

		_extent = extent;
		_imageFormat = surfaceFormat.format;
	}

	void VulkanEngine::InitImageViewInfos(
			::VkImageViewCreateInfo* _imageViewCreateInfo,
			::VkImage _swapChainImage,
			::VkFormat _swapChainImageFormat
	                                     )
	{
		_imageViewCreateInfo->sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		_imageViewCreateInfo->image = _swapChainImage;
		_imageViewCreateInfo->viewType = VK_IMAGE_VIEW_TYPE_2D;
		_imageViewCreateInfo->format = _swapChainImageFormat;
		_imageViewCreateInfo->components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		_imageViewCreateInfo->components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		_imageViewCreateInfo->components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		_imageViewCreateInfo->components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		_imageViewCreateInfo->subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		_imageViewCreateInfo->subresourceRange.baseMipLevel = 0;
		_imageViewCreateInfo->subresourceRange.levelCount = 1;
		_imageViewCreateInfo->subresourceRange.baseArrayLayer = 0;
		_imageViewCreateInfo->subresourceRange.layerCount = 1;
	}


	void VulkanEngine::InitRenderPassInfos(
			::VkAttachmentDescription* _colorAttachment,
			::VkAttachmentReference* _colorAttachmentRef,
			::VkSubpassDescription* _subpass,
			::VkRenderPassCreateInfo* _renderPassInfo,
			::VkFormat _swapChainImageFormat,
			::VkSubpassDependency* _subpassDependency
	                                      )
	{
		_colorAttachment->format = _swapChainImageFormat;
		_colorAttachment->samples = VK_SAMPLE_COUNT_1_BIT;
		_colorAttachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		_colorAttachment->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		_colorAttachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		_colorAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		_colorAttachment->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		_colorAttachment->finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		_colorAttachmentRef->attachment = 0;
		_colorAttachmentRef->layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		_subpass->pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		_subpass->colorAttachmentCount = 1;
		_subpass->pColorAttachments = _colorAttachmentRef;

		_renderPassInfo->sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		_renderPassInfo->attachmentCount = 1;
		_renderPassInfo->pAttachments = _colorAttachment;
		_renderPassInfo->subpassCount = 1;
		_renderPassInfo->pSubpasses = _subpass;
		_renderPassInfo->dependencyCount = 1;
		_renderPassInfo->pDependencies = _subpassDependency;

		_subpassDependency->srcSubpass = VK_SUBPASS_EXTERNAL;
		_subpassDependency->srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		_subpassDependency->srcAccessMask = 0;
		_subpassDependency->dstSubpass = 0;
		_subpassDependency->dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		_subpassDependency->dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}


	::VkResult VulkanEngine::CreateDebugUtilsMessengerEXT(
			::VkInstance _instance,
			const ::VkDebugUtilsMessengerCreateInfoEXT* _pCreateInfo,
			const ::VkAllocationCallbacks* _pAllocator,
			::VkDebugUtilsMessengerEXT* _pDebugMessenger
	                                                     )
	{
		auto func = (::PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(_instance,
		                                                                         "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(_instance, _pCreateInfo, _pAllocator, _pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void VulkanEngine::DestroyDebugUtilsMessengerEXT(
			::VkInstance _instance,
			::VkDebugUtilsMessengerEXT _debugMessenger,
			::VkAllocationCallbacks* _pAllocator
	                                                )
	{
		auto func = reinterpret_cast<::PFN_vkDestroyDebugUtilsMessengerEXT>(
				::vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT")
		);
		if (func != nullptr)
		{
			func(_instance, _debugMessenger, _pAllocator);
		}
	}

	void VulkanEngine::RecordCommandBuffer(
			::VkCommandBuffer _commandBuffer,
			::VkRenderPass _renderPass,
			::VkFramebuffer _framebuffer,
			::VkExtent2D _swapChainExtent,
			::VkPipeline _graphicsPipeline
	                                      )
	{
		::VkCommandBufferBeginInfo beginInfo { };
		::VkRenderPassBeginInfo renderPassInfo { };
		::VkResult result;

		::VkClearValue clearColor = {
				0.5f,
				0.5f,
				0.5f,
				1.0f
		};

		//TODO: move to a separate function
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		result = ::vkBeginCommandBuffer(_commandBuffer, &beginInfo);
		RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to begin recording command buffer!")

		//TODO: move to a separate function
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = _renderPass;
		renderPassInfo.framebuffer = _framebuffer;
		renderPassInfo.renderArea.offset = {
				0,
				0
		};
		renderPassInfo.renderArea.extent = _swapChainExtent;
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		::vkCmdBeginRenderPass(_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		::vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);
		::vkCmdDraw(_commandBuffer, 3, 1, 0, 0);
		::vkCmdEndRenderPass(_commandBuffer);

		result = ::vkEndCommandBuffer(_commandBuffer);
		RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to record command buffer!")
	}

	void VulkanEngine::DrawFrame()
	{
		::uint32_t imageIndex;
		::VkSubmitInfo submitInfo { };
		::VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
		::VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
		::VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		::VkPresentInfoKHR presentInfo { };
		::VkSwapchainKHR swapChains[] = { m_swapChain };
		::VkResult result;

		::vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);
		::vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);

		result = ::vkAcquireNextImageKHR(m_device, m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame],
		                                 VK_NULL_HANDLE,
		                                 &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}
		else
		{
			RUNTIME_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swap chain image!")
		}

		::vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);

		::vkResetCommandBuffer(m_commandBuffers[m_currentFrame], 0);

		RecordCommandBuffer(m_commandBuffers[m_currentFrame], m_renderPass, m_swapChainFramebuffers[imageIndex],
		                    m_swapChainExtent,
		                    m_graphicsPipeline);

		//TODO: move to a separate function
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		result = ::vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]);
		RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to submit draw command buffer!")

		//TODO: move to a separate function
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		result = ::vkQueuePresentKHR(m_presentQueue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized)
		{
			m_framebufferResized = false;
			RecreateSwapChain();
		}
		else
		{
			RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to present swap chain image!")
		}

		m_currentFrame = (m_currentFrame + 1) % VulkanEngine::MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanEngine::InitInstance()
	{
		::VkApplicationInfo appInfo { };
		::VkInstanceCreateInfo createInfo { };
		::VkResult result;

		InitInstanceInfos(&appInfo, &createInfo, m_vkValidationLayers);

		result = ::vkCreateInstance(&createInfo, nullptr, &m_vkInstance);
		RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create Instance")
	}

	void VulkanEngine::SetupDebugMessenger()
	{
#if PEPPER_VULKAN_VALIDATE_LAYERS
		::VkDebugUtilsMessengerCreateInfoEXT createInfo { };
		::VkResult result;

		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;
		createInfo.pUserData = nullptr;

		result = CreateDebugUtilsMessengerEXT(m_vkInstance, &createInfo, nullptr, &m_debugMessenger);
		RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to set up debug messenger!")
#endif
	}

	void VulkanEngine::DestroyDebugMessenger()
	{
#if PEPPER_VULKAN_VALIDATE_LAYERS
		DestroyDebugUtilsMessengerEXT(m_vkInstance, m_debugMessenger, nullptr);
#endif
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

	void VulkanEngine::CreateLogicalDevice()
	{
		QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice, m_surface);
		::VkDeviceCreateInfo deviceCreateInfo { };
		::VkPhysicalDeviceFeatures deviceFeatures = {};
		::VkResult result;
		std::vector<::VkDeviceQueueCreateInfo> queueCreateInfos;

		RUNTIME_ASSERT(indices.IsComplete(), "Failed to find suitable queue families")
		RUNTIME_ASSERT(IsDeviceSuitable(m_physicalDevice, m_deviceExtensions, m_surface),
		               "Failed to find a suitable GPU")

		InitDeviceInfos(indices, &queueCreateInfos, &deviceCreateInfo, &deviceFeatures, m_deviceExtensions, m_vkValidationLayers);

		result = ::vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device);
		RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create logical device")

		::vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
		::vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
	}

	void VulkanEngine::CreateSwapChain()
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_physicalDevice, m_surface);
		::VkSwapchainCreateInfoKHR createInfo { };
		::uint32_t swapChainImageCount;
		::VkResult result;

		InitSwapChainInfos(swapChainSupport, &createInfo, m_glWindow,
		                   m_physicalDevice, m_surface, m_swapChainExtent,
		                   m_swapChainImageFormat);

		result = ::vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain);
		RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create swap chain")

		result = ::vkGetSwapchainImagesKHR(m_device, m_swapChain, &swapChainImageCount, nullptr);
		RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get swap chain images count")

		m_swapChainImages.resize(swapChainImageCount);
		result = ::vkGetSwapchainImagesKHR(m_device, m_swapChain, &swapChainImageCount, m_swapChainImages.data());
		RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to get swap chain images")
	}

	void VulkanEngine::CleanupSwapChain()
	{
		for (auto framebuffer: m_swapChainFramebuffers)
		{
			::vkDestroyFramebuffer(m_device, framebuffer, nullptr);
		}

		for (auto &imageView: m_swapChainImageViews)
		{
			::vkDestroyImageView(m_device, imageView, nullptr);
		}

		::vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
	}

	void VulkanEngine::RecreateSwapChain()
	{
		int width = 0, height = 0;
		::glfwGetFramebufferSize(m_glWindow, &width, &height);

		while (width == 0 || height == 0)
		{
			::glfwGetFramebufferSize(m_glWindow, &width, &height);
			::glfwWaitEvents();
		}

		::vkDeviceWaitIdle(m_device);

		CleanupSwapChain();

		CreateSwapChain();
		CreateImageViews();
		CreateFramebuffers();
	}

	void VulkanEngine::CreateImageViews()
	{
		m_swapChainImageViews.resize(m_swapChainImages.size());

		for (::size_t i = 0; i < m_swapChainImages.size(); i++)
		{
			::VkImageViewCreateInfo createInfo { };
			::VkResult result;

			InitImageViewInfos(&createInfo, m_swapChainImages[i], m_swapChainImageFormat);

			result = ::vkCreateImageView(m_device, &createInfo, nullptr, &m_swapChainImageViews[i]);
			RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create image views")
		}
	}

	void VulkanEngine::CreateRenderPass()
	{
		::VkResult result;
		::VkAttachmentDescription colorAttachment { };
		::VkAttachmentReference colorAttachmentRef { };
		::VkSubpassDescription subpass { };
		::VkRenderPassCreateInfo renderPassInfo { };
		::VkSubpassDependency dependency { };

		InitRenderPassInfos(&colorAttachment, &colorAttachmentRef, &subpass, &renderPassInfo,
		                    m_swapChainImageFormat, &dependency);
		result = ::vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass);
		RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create render pass.")
	}

	void VulkanEngine::CreateGraphicsPipeline()
	{
		::VkPipelineShaderStageCreateInfo vertShaderStageInfo { };
		::VkPipelineShaderStageCreateInfo fragShaderStageInfo { };

		std::vector<char> vertShaderCode = ReadShaderFile("./Shaders/shader.vert.spv");
		std::vector<char> fragShaderCode = ReadShaderFile("./Shaders/shader.frag.spv");

		::VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode, m_device);
		::VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode, m_device);

		auto bindingDescription = Vertex::GetBindingDescription();
		auto attributeDescriptions = Vertex::GetAttributeDescriptions();

		::VkResult result;

		InitShaderStageInfos(&vertShaderStageInfo, vertShaderModule, VK_SHADER_STAGE_VERTEX_BIT);
		InitShaderStageInfos(&fragShaderStageInfo, fragShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT);

		::VkPipelineShaderStageCreateInfo shaderStages[] = {
				vertShaderStageInfo,
				fragShaderStageInfo
		};

		//TODO: move this to a separate function
		::VkPipelineVertexInputStateCreateInfo vertexInputInfo { };
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<::uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		//TODO: move this to a separate function
		::VkPipelineInputAssemblyStateCreateInfo inputAssembly { };
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		//TODO: move this to a separate function
		::VkViewport viewport { };
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float) m_swapChainExtent.width;
		viewport.height = (float) m_swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		//TODO: move this to a separate function
		::VkRect2D scissor { };
		scissor.offset = {
				0,
				0
		};
		scissor.extent = m_swapChainExtent;

		//TODO: dynamic states

		//TODO: move this to a separate function
		::VkPipelineViewportStateCreateInfo viewportState { };
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		//TODO: move this to a separate function
		::VkPipelineRasterizationStateCreateInfo rasterizer { };
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		//TODO: move this to a separate function
		::VkPipelineMultisampleStateCreateInfo multisampling { };
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		//TODO: move this to a separate function
		::VkPipelineColorBlendAttachmentState colorBlendAttachment { };
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
				| VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		//TODO: move this to a separate function
		::VkPipelineColorBlendStateCreateInfo colorBlending { };
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		//TODO: move this to a separate function
		::VkPipelineLayoutCreateInfo pipelineLayoutInfo { };
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0; // Optional
		pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		result = vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout);
		RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create pipeline layout.")

		::VkGraphicsPipelineCreateInfo pipelineInfo { };
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr; // Optional
		pipelineInfo.layout = m_pipelineLayout;
		pipelineInfo.renderPass = m_renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		result = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline);
		RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create graphics pipeline.")

		::vkDestroyShaderModule(m_device, fragShaderModule, nullptr);
		::vkDestroyShaderModule(m_device, vertShaderModule, nullptr);
	}

	void VulkanEngine::CreateFramebuffers()
	{
		::VkResult result;
		m_swapChainFramebuffers.resize(m_swapChainImageViews.size());
		for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
		{
			::VkImageView attachments[] = {
					m_swapChainImageViews[i]
			};

			//TODO: move this to a separate function
			::VkFramebufferCreateInfo framebufferInfo { };
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = m_swapChainExtent.width;
			framebufferInfo.height = m_swapChainExtent.height;
			framebufferInfo.layers = 1;

			result = vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]);
			RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create framebuffer.")
		}
	}

	void VulkanEngine::CreateCommandPool()
	{
		::VkResult result;
		::VkCommandPoolCreateInfo poolInfo { };
		QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_physicalDevice, m_surface);

		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		result = vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool);
		RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create command pool.")
	}

	void VulkanEngine::CreateCommandBuffers()
	{
		::VkCommandBufferAllocateInfo allocInfo { };
		::VkResult result;

		//TODO: move this to a separate function
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = m_commandBuffers.size();

		result = vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data());
		RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to allocate command buffer.")
	}

	void VulkanEngine::CreateSyncObjects()
	{
		::VkSemaphoreCreateInfo semaphoreInfo { };
		::VkFenceCreateInfo fenceInfo { };
		::VkResult result;

		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (int i = 0; i < VulkanEngine::MAX_FRAMES_IN_FLIGHT; ++i)
		{
			result = ::vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]);
			RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create image available semaphore.")
			result = ::vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]);
			RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create render finished semaphore.")
			result = ::vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]);
			RUNTIME_ASSERT(result == VK_SUCCESS, "Failed to create in flight fence.")
		}
	}

	void VulkanEngine::Init(
			int _width,
			int _height
	                       )
	{
		int result;

		result = ::glfwInit();
		RUNTIME_ASSERT(result == GLFW_TRUE, "Failed to initialize GLFW.")

		::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		//It's the parent that will take care of resizing, but we still need to handle some callbacks
		::glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		//TODO: set GLFW_DECORATED to GLFW_FALSE
		::glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
		::glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		m_glWindow = ::glfwCreateWindow(_width, _height, "ZWP", nullptr, nullptr);
		RUNTIME_ASSERT(m_glWindow != nullptr, "Failed to create GLFW window.")

		::glfwSetWindowUserPointer(m_glWindow, this);
		::glfwSetFramebufferSizeCallback(m_glWindow, VulkanEngine::FramebufferResizeCallback);

#   if PEPPER_VULKAN_VALIDATE_LAYERS
		InitValidationLayers();
#   endif

		InitInstance();
		SetupDebugMessenger();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateGraphicsPipeline();
		CreateFramebuffers();
		CreateCommandPool();
		CreateCommandBuffers();
		CreateSyncObjects();
	}

	void VulkanEngine::Update()
	{
		::glfwPollEvents();
		DrawFrame();
	}

	void VulkanEngine::Shutdown()
	{
		//PLACE FIRST
		::vkDeviceWaitIdle(m_device);

		CleanupSwapChain();

		::vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
		::vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);

		::vkDestroyRenderPass(m_device, m_renderPass, nullptr);

		for (int i = 0; i < VulkanEngine::MAX_FRAMES_IN_FLIGHT; ++i)
		{
			::vkDestroySemaphore(m_device, m_renderFinishedSemaphores[i], nullptr);
			::vkDestroySemaphore(m_device, m_imageAvailableSemaphores[i], nullptr);
			::vkDestroyFence(m_device, m_inFlightFences[i], nullptr);
		}

		::vkDestroyCommandPool(m_device, m_commandPool, nullptr);

		::vkDestroyDevice(m_device, nullptr);

		::vkDestroySurfaceKHR(m_vkInstance, m_surface, nullptr);

#if PEPPER_VULKAN_VALIDATE_LAYERS
		DestroyDebugMessenger();
#endif

		::vkDestroyInstance(m_vkInstance, nullptr);

		::glfwDestroyWindow(m_glWindow);

		::glfwTerminate();
	}

	void* VulkanEngine::GetWindow() const
	{
		RUNTIME_ASSERT(m_glWindow != nullptr, "Failed to get GLFW window. Engine not initialized?")
		return static_cast<void*>(m_glWindow);
	}

	bool VulkanEngine::ShouldClose()
	{
		RUNTIME_ASSERT(m_glWindow != nullptr, "Failed to get GLFW window. Engine not initialized?")
		return ::glfwWindowShouldClose(m_glWindow);
	}

	EngineBase::EngineType VulkanEngine::GetType() const
	{
		RUNTIME_ASSERT(m_glWindow != nullptr, "Failed to get window. Is Engine initialized?")
		return EngineBase::EngineType::Vulkan;
	}
}