#include <clocale>
#include <exception>
#include <iostream>
#include "Wallpaper.h" //platform-specific location
#include "glfw/glfw3.h"


class HelloTriangleApplication {
public:
    void run() {
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;

    void initVulkan() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(800, 600, "Vulkan Test Window", nullptr, nullptr);
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
    HDC testDrawSurface;

#pragma region TestVulkan
    {
        HelloTriangleApplication app{};
        try {
            app.run();
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
        RECT surface;
        HWND hwnd = ::WindowFromDC(testDrawSurface);
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
        SHORT escapeKeyState = GetAsyncKeyState(VK_ESCAPE) & 0x8000;
        SHORT actualKeyState = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
        if (actualKeyState != previousKeyState) {
            printf("Oui");
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
