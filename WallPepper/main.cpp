#include <clocale>
#include "../WindowsImpl/Wallpaper.h"

int main() {
    Pepper::Impl::Wallpaper wp;
    HDC testDrawSurface;

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
