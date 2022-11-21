//
// Created by ZoSand on 21/11/2022.
//
#include <stdexcept>
#include "Wallpaper.h"

Win32::HWND Pepper::Impl::Wallpaper::GetWallpaperHandle(bool _secondRun = false) {
    Win32::HWND handle = Win32::FindWindow(L"Progman", nullptr);
    if (handle == nullptr) {
        throw std::runtime_error("Unable to find Progman");
    }

    (void) Win32::SendMessage(handle, 0x052C, 0xD, 0);
    (void) Win32::SendMessage(handle, 0x052C, 0xD, 1);

    Win32::EnumWindows([](Win32::HWND _hWnd, Win32::LPARAM _lParam) -> Win32::BOOL {

        Win32::HWND shellHandle = Win32::FindWindowEx(_hWnd, nullptr, L"SHELLDLL_DefView", nullptr);
        if (shellHandle != nullptr) {
            auto *hnd = reinterpret_cast<Win32::HWND *>(_lParam);
            *hnd = Win32::FindWindowEx(nullptr, _hWnd, L"WorkerW", nullptr);
            _lParam = reinterpret_cast<Win32::LPARAM>(*hnd);
            if (*hnd != nullptr) {
                return FALSE;
            }
        }
        return TRUE;
    }, reinterpret_cast<Win32::LPARAM>(&handle));

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
