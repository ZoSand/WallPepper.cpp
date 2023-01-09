//
// Created by ZoSand on 24/11/2022.
//

#include <iostream>
#include <windows.h>
#include <chrono>
#include "Application.h"

Pepper::Main::Application::Application()
        : m_wp(nullptr)
{
}

Pepper::Main::Application::~Application() = default;

void Pepper::Main::Application::Init(Core::EngineBase::EngineType _engineType = Core::EngineBase::EngineType::Vulkan)
{
    m_wp = new Pepper::Impl::Wallpaper();
	m_wp->SetEngine(_engineType);
    m_wp->Init();
}

void Pepper::Main::Application::Run()
{
	Core::Drawable d = Core::Drawable();
    while (!m_wp->ShouldClose())
    {
        m_wp->Update();
		m_wp->Clear(0.0f, 0.0f, 0.0f, 1.0f);
		m_wp->Draw(d);
		m_wp->Render();
    }
}

void Pepper::Main::Application::Clear()
{
    m_wp->Shutdown();
    delete m_wp;
}
