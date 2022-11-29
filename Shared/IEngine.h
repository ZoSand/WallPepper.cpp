//
// Created by ZoSand on 29/11/2022.
//

#ifndef PEPPER_SHARED_IENGINE_H
#define PEPPER_SHARED_IENGINE_H

#include "ConstantDefinitions.h" //should always be placed first

namespace Pepper::Core
{
    class IEngine
    {
    public:
        enum class EngineType
        {
            Vulkan,
            OpenGL,
            DirectX
        };

        virtual ~IEngine() = default;

        NO_DISCARD virtual bool ShouldClose() = 0;

        NO_DISCARD_UNUSED virtual void *GetWindow() const = 0;

        NO_DISCARD_UNUSED virtual EngineType GetType() const = 0;

        virtual void Init(int, int) = 0;

        virtual void Update() = 0;

        virtual void Shutdown() = 0;
    };
}

#endif //PEPPER_SHARED_IENGINE_H
