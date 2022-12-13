//
// Created by ZoSand on 21/11/2022.
//
#include <ConstantDefinitions.h> //should always be placed first
#include <stdexcept>
#include "Wallpaper.h"

#include "GLFW/glfw3.h"

#define GLFW_EXPOSE_NATIVE_WIN32

#include "GLFW/glfw3native.h"

namespace Pepper::Impl
{
//CLion says this is a recursive chain loop, but it's secured by the _secondRun bool
	::HWND Wallpaper::GetWallpaperHandle(bool _secondRun = false)
	{
		::HWND handle = ::FindWindow(L"Progman", nullptr);
		RUNTIME_ASSERT(handle != nullptr, "Failed to locate Progman window")

		(void) ::SendMessage(handle, 0x052C, 0xD, 0);
		(void) ::SendMessage(handle, 0x052C, 0xD, 1);

		::EnumWindows([](
				::HWND _hWnd,
				::LPARAM _lParam
		                ) -> ::BOOL
		              {

			              ::HWND shellHandle = ::FindWindowEx(_hWnd, nullptr, L"SHELLDLL_DefView", nullptr);
			              if (shellHandle != nullptr)
			              {
				              auto* hnd = reinterpret_cast<::HWND*>(_lParam);
				              *hnd = ::FindWindowEx(nullptr, _hWnd, L"WorkerW", nullptr);
				              _lParam = reinterpret_cast<::LPARAM>(*hnd); //Don't remove this line even if IDE says it's useless
				              if (*hnd != nullptr)
				              {
					              return FALSE;
				              }
			              }
			              return TRUE;
		              }, reinterpret_cast<::LPARAM>(&handle));

		if (handle == nullptr && !_secondRun)
		{
			handle = GetWallpaperHandle(true); //sometimes it takes 2 runs to find the window
			//I mean like... Duh
		}

		return handle;
	}

	Wallpaper::Wallpaper()
			: Core::Window()
	{
		::HWND wnd = GetWallpaperHandle();
		Core::Window::SetWindow((void*) (wnd));
	}

	Wallpaper::~Wallpaper() = default;

	bool Wallpaper::ShouldClose()
	{
		return Core::Window::GetEngine()->ShouldClose();
	}

	void Wallpaper::Init()
	{
		::RECT wndRect;
		::GetWindowRect(static_cast<::HWND>(Core::Window::GetWindow()), &wndRect);
		Core::EngineBase* engine = Core::Window::GetEngine();
		engine->Init(wndRect.right - wndRect.left, wndRect.bottom - wndRect.top);
		switch (engine->GetType())
		{
			case Core::EngineBase::EngineType::Vulkan:
			case Core::EngineBase::EngineType::OpenGL:
			{
				::SetParent(
				        glfwGetWin32Window(static_cast<::GLFWwindow *>(engine->GetWindow())),
				        static_cast<::HWND>(Core::Window::GetWindow()));
				::ShowWindow(glfwGetWin32Window(static_cast<::GLFWwindow*>(engine->GetWindow())), SW_SHOW);
				break;
			}
			case Core::EngineBase::EngineType::DirectX:
			default:
				throw std::runtime_error("Engine type not supported");
		}
	}

	void Wallpaper::Update()
	{
		Core::Window::UpdateInternal();
	}

	void Wallpaper::Shutdown()
	{
		Core::Window::Shutdown();
	}

}