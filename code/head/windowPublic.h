#pragma once

#include <windows.h>
#include "Application.h"
#include "GraphicEngineProc.h"

/*
    Define function of all window public
*/

#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx12.h>
#include "resource.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ViewApplication
{
    //  Call D3DCore wnd to process message
    void D3DMsgProcAndResultWndProc(UINT message, WPARAM wParam, LPARAM lParam);

    //  Called when a child window is creates
    void OnCreate(HWND childHwnd);

    // Called when the sub window size is adjusted
    void OnSizing(int index, WPARAM wParam, LPARAM lParam);

    // Determine the stretching area of the window
    LRESULT OnNCHITTEST(int index, LPARAM lParam);

}