//
// Created by ZoSand on 24/11/2022.
//

#ifndef WALLPEPPER_APPLICATION_H
#define WALLPEPPER_APPLICATION_H
//  platform-specific header;
// dynamic library compiled from LinuxImpl or WindowsImpl
#include <Wallpaper.h>

namespace Pepper::App {
    class Application {
    private:
        Pepper::Impl::Wallpaper *m_wp;
    public:
        Application();
        ~Application();

        void Init();
        void Run();
        void Clear();
    };
}

#endif //WALLPEPPER_APPLICATION_H
