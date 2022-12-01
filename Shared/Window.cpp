//
// Created by ZoSand on 21/11/2022.
//

#include <stdexcept>
#include "Window.h"
#include "VulkanEngine.h"

namespace Pepper::Core {

    Window::Window()
            : m_engine(nullptr), m_handle(nullptr) {
    }

    Window::~Window()
    = default;

    void Window::SetWindow(void *_window) {
        m_handle = _window;
    }

    void *Window::GetWindow() const {
        return m_handle;
    }

    void Window::SetEngineInternal(IEngine *_engine) {
        if (m_engine != nullptr) {
            m_engine->Shutdown();
            delete m_engine;
        }

        m_engine = _engine;
    }

    void Window::SetEngine(IEngine::EngineType _engineType) {
        switch (_engineType) {
            case IEngine::EngineType::Vulkan:
                SetEngineInternal(new VulkanEngine());
                break;
            case IEngine::EngineType::OpenGL: //new OpenGLEngine();
            case IEngine::EngineType::DirectX: //new DirectXEngine();
            default:
                throw std::runtime_error("Engine type not supported");
        }
    }

    IEngine *Window::GetEngine() {
        RUNTIME_ASSERT(m_engine != nullptr, "Engine is not set")
        return m_engine;
    }

    void Window::UpdateInternal() {
        RUNTIME_ASSERT(m_engine != nullptr, "Engine is not set")
        m_engine->Update();
    }

    void Window::Shutdown() {
        RUNTIME_ASSERT(m_engine != nullptr, "Engine is not set")
        m_engine->Shutdown();
        delete m_engine;
        m_engine = nullptr;
    }
}