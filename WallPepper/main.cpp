#include <clocale>
#include <exception>
#include <iostream>

#include "Application.h"

int main() {
    //var declaration
    Pepper::App::Application app;
    int status = EXIT_SUCCESS;

    //init
    ::setlocale(LC_ALL, "");
    app.Init();

    //run loop
    try {
        app.Run();
    }
    catch (std::exception &_e) {
        std::cerr << _e.what() << std::endl;
        status = EXIT_FAILURE;
    }

    //cleanup
    app.Clear();

    return status;
}
/*
    Pepper::Impl::Wallpaper wp;
    ::HDC testDrawSurface;

/*
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
//FIN DU PREMIER COMMENTAIRE

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
*/
