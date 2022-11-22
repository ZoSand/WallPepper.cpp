#include <clocale>
#include <exception>
#include <iostream>


#include <Wallpaper.h> //platform-specific location // dynamic library compiled from LinuxImpl or WindowsImpl

//#define GLFW_INCLUDE_VULKAN
#define VK_USE_PLATFORM_WIN32_KHR
#include <glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>


class HelloTriangleApplication {
public:
    void run(Pepper::Impl::Wallpaper& wp) {
        initVulkan(wp);
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;

    void initVulkan(Pepper::Impl::Wallpaper& wp) {
        ::RECT windowRect;

        ::GetWindowRect((HWND)wp.GetWindow(), &windowRect);

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        window = glfwCreateWindow(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
                                  "Vulkan Test Window", nullptr, nullptr);
        ::SetParent(glfwGetWin32Window(window), (HWND)wp.GetWindow());
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        glfwDestroyWindow(window);

        glfwTerminate();
    }
};

int main() {
    Pepper::Impl::Wallpaper wp;
    ::HDC testDrawSurface;

#pragma region TestVulkan
    {
        ::HelloTriangleApplication app{};
        try {
            app.run(wp);
        } catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }
    }
#pragma endregion

    ::setlocale(LC_ALL, "");

    testDrawSurface = ::GetDCEx((::HWND)wp.GetWindow(), nullptr, 0x403);
    if (testDrawSurface != nullptr)
    {
        ::RECT surface;
        ::HWND hwnd = ::WindowFromDC(testDrawSurface);
        ::GetWindowRect(hwnd, &surface);

        surface.right = surface.right / 2;

        ::FillRect(testDrawSurface, &surface, ::CreateSolidBrush(0x000000FF));

        surface.left += surface.right;
        surface.right *= 2;

        ::FillRect(testDrawSurface, &surface, ::CreateSolidBrush(0x0000FF00));
        ::InvalidateRect(hwnd, nullptr, TRUE);
        ::ReleaseDC(hwnd, testDrawSurface);
    }

    SHORT previousKeyState = 0;

    for (;;)
    {
        ::SHORT escapeKeyState = ::GetAsyncKeyState(VK_ESCAPE) & 0x8000;
        ::SHORT actualKeyState = ::GetAsyncKeyState(VK_LBUTTON) & 0x8000;
        if (actualKeyState != previousKeyState) {
            ::printf("Oui");
        }
        previousKeyState = actualKeyState;
        if (escapeKeyState)
        {
            Pepper::Impl::Wallpaper _wp;
            break;
        }
        ::Sleep(1);
    }

    return 0;
}
