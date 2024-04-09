#pragma once

#include "UString.h"
#include "Application.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx12.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace Rendering;

namespace ViewApplication
{
	LRESULT CALLBACK OnlyWndProc(HWND, UINT, WPARAM, LPARAM);

	class OnlyWindow
	{
	public:
		OnlyWindow() {}

		void Build(HINSTANCE hInstance, TCHAR* appName);

		HWND GetHWND();

	private:
		RECT lastInfo;
		HWND hwnd;
	};
}