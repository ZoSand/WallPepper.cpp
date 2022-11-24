//
// Created by ZoSand on 24/11/2022.
//

#include "Application.h"

Pepper::App::Application::Application()
        : m_wp(nullptr) {

}

Pepper::App::Application::~Application() = default;

void Pepper::App::Application::Run() {
    m_wp->Update();
}

void Pepper::App::Application::Init() {
    m_wp = new Pepper::Impl::Wallpaper();
}

void Pepper::App::Application::Clear() {
    delete m_wp;
}
