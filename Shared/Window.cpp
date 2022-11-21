//
// Created by ZoSand on 21/11/2022.
//

#include "Window.h"

[[maybe_unused]] void Pepper::Shared::Window::SetWindow(void *_window) {
    m_handle = _window;
}

Pepper::Shared::Window::Window()
        : m_handle(nullptr) {

}

[[maybe_unused]] void *Pepper::Shared::Window::GetWindow() {
    return m_handle;
}

Pepper::Shared::Window::~Window() = default;
