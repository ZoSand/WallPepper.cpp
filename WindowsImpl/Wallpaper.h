//
// Created by ZoSand on 21/11/2022.
//

#ifndef WINDOWSIMPL_WALLPEPPER_WALLPAPER_H
#define WINDOWSIMPL_WALLPEPPER_WALLPAPER_H

//THIS LIBRARY IMPORT
#include "library.h"

//SHARED LIBRARY DEFINITIONS
#include <Window.h>

//PLATFORM API
#include <windows.h>

namespace Pepper::Impl
{
    class PEPPER_DYNAMIC_LIB Wallpaper
            : public Pepper::Core::Window
    {
    private:
        NO_DISCARD static ::HWND GetWallpaperHandle(bool _secondRun);

    public:

        Wallpaper();

        ~Wallpaper();

        bool ShouldClose();

        void Init();

        void Update();

        void Shutdown() final;
    };
}


#endif //WINDOWSIMPL_WALLPEPPER_WALLPAPER_H
