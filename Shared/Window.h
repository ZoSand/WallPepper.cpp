//
// Created by ZoSand on 21/11/2022.
//

#ifndef SHARED_WALLPEPPER_WINDOW_H
#define SHARED_WALLPEPPER_WINDOW_H

//THIS LIBRARY DEFINITIONS
#   include "library.h"

//GLFW dependencies
#   define GLFW_INCLUDE_VULKAN
#   define GLFW_EXPOSE_NATIVE_WIN32

#   include <glfw/glfw3.h>
#   include <GLFW/glfw3native.h>

//other includes
#   include <vector>
#   include <optional>

namespace Pepper::Shared
{
    class PEPPER_SHARED_LIB Window
    {
    private:
        void *m_handle;
        ::GLFWwindow *m_glWindow{};
        ::VkInstance m_vkInstance;
        ::VkPhysicalDevice m_physicalDevice;
        ::VkDevice m_device;
        ::VkQueue m_graphicsQueue;

#   if PEPPER_VULKAN_VALIDATE_LAYERS
        std::vector<const char *> m_vkValidationLayers;

        void InitValidationLayers();

#   endif

        struct QueueFamilyIndices
        {
            std::optional<::uint32_t> graphicsFamily;

            [[maybe_unused]] [[nodiscard]] bool IsComplete() const
            {
                return graphicsFamily.has_value();
            }
        };

        [[nodiscard]] static ::uint32_t RateDeviceSuitability(::VkPhysicalDevice _device);

        [[nodiscard]] static QueueFamilyIndices FindQueueFamilies(::VkPhysicalDevice _device);

        void InitInstanceInfos(
                ::VkApplicationInfo *_appInfo,
                ::VkInstanceCreateInfo *_createInfo
                              );

        void InitInstance();

        void PickPhysicalDevice();

        void InitDeviceInfos(
                QueueFamilyIndices _indices,
                ::VkDeviceQueueCreateInfo *_queueCreateInfo,
                ::VkDeviceCreateInfo *_deviceCreateInfo
                            );

        void CreateLogicalDevice();

    protected:
        [[maybe_unused]] void SetWindow(void *_window);

        [[maybe_unused]] void Init(int _width, int _height);

    public:
        Window();

        ~Window();

        [[maybe_unused]] [[nodiscard]] void *GetWindow() const;

        [[maybe_unused]] [[nodiscard]] GLFWwindow *GetGlWindow() const;

        [[maybe_unused]] void Update();

        [[maybe_unused]] void Clear();
    };
}

#endif //SHARED_WALLPEPPER_WINDOW_H
