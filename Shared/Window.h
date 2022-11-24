//
// Created by ZoSand on 21/11/2022.
//

#ifndef SHARED_WALLPEPPER_WINDOW_H
#define SHARED_WALLPEPPER_WINDOW_H

//THIS LIBRARY DEFINITIONS
#include "library.h"

//GLFW dependencies
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32

#include <glfw/glfw3.h>
#include <GLFW/glfw3native.h>


namespace Pepper::Shared {
    class EXPORTED Window {
    private:
        void *m_handle;
        ::GLFWwindow *m_glWindow{};
        ::VkInstance m_vkInstance{};

    protected:
        [[maybe_unused]] void SetWindow(void *_window);

        void InitInstance();

        [[maybe_unused]] void Init(int _width, int _height);

    public:
        Window();

        ~Window();

        [[maybe_unused]] [[nodiscard]] void *GetWindow() const;

        [[maybe_unused]] [[nodiscard]] GLFWwindow *GetGlWindow() const;

        [[maybe_unused]] void Update();
    };
}

#endif //SHARED_WALLPEPPER_WINDOW_H
