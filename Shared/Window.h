//
// Created by ZoSand on 21/11/2022.
//

#ifndef SHARED_WALLPEPPER_WINDOW_H
#define SHARED_WALLPEPPER_WINDOW_H
#include "library.h"

namespace Pepper::Shared {
    class EXPORTED Window {
    private:
        void *m_handle;
    protected:
        [[maybe_unused]] void SetWindow(void *_window);

    public:
        Window();
        ~Window();

        [[maybe_unused]] void *GetWindow();
    };
}

#endif //SHARED_WALLPEPPER_WINDOW_H
