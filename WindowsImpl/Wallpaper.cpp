//
// Created by ZoSand on 21/11/2022.
//
#include <stdexcept>
#include "Wallpaper.h"
//CLion says this is a recursive loop, but it's secured by the _secondRun bool
::HWND Pepper::Impl::Wallpaper::GetWallpaperHandle(bool _secondRun = false) {
    ::HWND handle = ::FindWindow(L"Progman", nullptr);
    if (handle == nullptr) {
        throw std::runtime_error("Unable to find Progman");
    }

    (void) ::SendMessage(handle, 0x052C, 0xD, 0);
    (void) ::SendMessage(handle, 0x052C, 0xD, 1);

    ::EnumWindows([](::HWND _hWnd, ::LPARAM _lParam) -> ::BOOL {

        ::HWND shellHandle = ::FindWindowEx(_hWnd, nullptr, L"SHELLDLL_DefView", nullptr);
        if (shellHandle != nullptr) {
            auto *hnd = reinterpret_cast<::HWND *>(_lParam);
            *hnd = ::FindWindowEx(nullptr, _hWnd, L"WorkerW", nullptr);
            _lParam = reinterpret_cast<::LPARAM>(*hnd); //Don't remove this line event if IDE says it's useless
            if (*hnd != nullptr) {
                return FALSE;
            }
        }
        return TRUE;
    }, reinterpret_cast<::LPARAM>(&handle));

    if (handle == nullptr && !_secondRun) {
        handle = GetWallpaperHandle(true); //sometimes it takes 2 runs to find the window
        //I mean like... Duh
    }

    return handle;
}

Pepper::Impl::Wallpaper::Wallpaper()
        : Pepper::Shared::Window() {
    ::HWND wnd = GetWallpaperHandle();
    ::RECT wndRect;
    ::GetWindowRect(wnd, &wndRect);
    Pepper::Shared::Window::SetWindow((void*)(wnd));
    Pepper::Shared::Window::Init(wndRect.right - wndRect.left, wndRect.bottom - wndRect.top);
    //::SetParent(glfwGetWin32Window(Pepper::Shared::Window::GetGlWindow()), wnd);
}

Pepper::Impl::Wallpaper::~Wallpaper() {
    Pepper::Shared::Window::~Window();
};