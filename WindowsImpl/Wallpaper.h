//
// Created by ZoSand on 21/11/2022.
//

#ifndef WINDOWSIMPL_WALLPEPPER_WALLPAPER_H
#define WINDOWSIMPL_WALLPEPPER_WALLPAPER_H

#include "../Shared/Window.h"

#ifndef UNICODE
#define UNICODE
#endif

namespace Win32{
#include <windows.h>
}
namespace Pepper::Impl {
    class EXPORTED Wallpaper

    : public Pepper::Shared::Window {
    private:
        static Win32::HWND GetWallpaperHandle(bool _secondRun);

    public:

    Wallpaper();

    ~

    Wallpaper();
};
}


#endif //WINDOWSIMPL_WALLPEPPER_WALLPAPER_H
