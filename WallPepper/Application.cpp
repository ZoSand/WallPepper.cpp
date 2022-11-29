//
// Created by ZoSand on 24/11/2022.
//

#include <iostream>
#include <windows.h>
#include "Application.h"

Pepper::Main::Application::Application()
        : m_wp(nullptr)
{
}

Pepper::Main::Application::~Application() = default;

void Pepper::Main::Application::Init()
{
    m_wp = new Pepper::Impl::Wallpaper();
    m_wp->Init();
}

void Pepper::Main::Application::Run()
{
    while (!m_wp->ShouldClose())
    {
        m_wp->Update();
    }
}

void Pepper::Main::Application::Clear()
{
    m_wp->Shutdown();
    delete m_wp;
}
