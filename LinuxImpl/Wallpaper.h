//
// Created by ZoSand on 21/11/2022.
//

#ifndef LINUXIMPL_WALLPEPPER_WALLPAPER_H
#define LINUXIMPL_WALLPEPPER_WALLPAPER_H

#include <Window.h>

#ifndef UNICODE
#define UNICODE
#endif

namespace Pepper::Impl {
    class PEPPER_DYNAMIC_LIB Wallpaper
            : public Pepper::Shared::Window {
    public:
        Wallpaper();
        ~Wallpaper();
    };
}


#endif //LINUXIMPL_WALLPEPPER_WALLPAPER_H
