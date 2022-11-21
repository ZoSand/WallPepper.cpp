//
// Created by ZoSand on 21/11/2022.
//
#include <stdexcept>
#include "Wallpaper.h"

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
            _lParam = reinterpret_cast<::LPARAM>(*hnd);
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

Pepper::Impl::Wallpaper::Wallpaper() {
    SetWindow(GetWallpaperHandle());
}

Pepper::Impl::Wallpaper::~Wallpaper() = default;
