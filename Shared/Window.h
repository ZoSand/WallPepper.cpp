//
// Created by ZoSand on 21/11/2022.
//

#ifndef SHARED_WALLPEPPER_WINDOW_H
#define SHARED_WALLPEPPER_WINDOW_H

//THIS LIBRARY DEFINITIONS
#   include "library.h"
#   include "IEngine.h"

namespace Pepper::Core
{
    class PEPPER_SHARED_LIB Window
    {
    private:
        void *m_handle;
        IEngine *m_engine;

        void SetEngineInternal(IEngine *_engine);

    protected:
        MAYBE_UNUSED void SetWindow(void *_window);

        void UpdateInternal();

    public:
        Window();

        ~Window();

        NO_DISCARD_UNUSED void *GetWindow() const;

        void SetEngine(IEngine::EngineType _engineType);

        IEngine *GetEngine();

        virtual void Shutdown();
    };
}

#endif //SHARED_WALLPEPPER_WINDOW_H
