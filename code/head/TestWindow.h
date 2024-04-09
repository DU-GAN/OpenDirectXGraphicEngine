#pragma once

#include <windows.h>

namespace GraphicEngine
{
    LRESULT CALLBACK TestWndProc(HWND, UINT, WPARAM, LPARAM);

	class TestWindow
	{
        void Build(HINSTANCE hInstance)
        {
            WNDCLASS wndclass;

            wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
            wndclass.lpfnWndProc = TestWndProc;
            wndclass.cbClsExtra = 0;
            wndclass.cbWndExtra = 0;
            wndclass.hInstance = hInstance;
            wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
            wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
            wndclass.hbrBackground = hBrush;
            wndclass.lpszMenuName = NULL;
            wndclass.lpszClassName = wndClassName;

            if (!RegisterClass(&wndclass))
            {
                MessageBox(NULL, TEXT("RigisterClass function failed:" + GetLastError()), TEXT("Error"), MB_ICONERROR);
                return;
            }

            HMENU hMenu = LoadMenu(hInstance, (LPCTSTR)(IDR_MENU1));

            MENUINFO menuInfo;
            menuInfo.cbSize = sizeof(menuInfo);
            menuInfo.fMask = MIM_BACKGROUND | MIM_APPLYTOSUBMENUS;
            menuInfo.hbrBack = CreateSolidBrush(RGB(0, 238, 224));

            mainHwnd = CreateWindow(wndClassName, appName,
                WS_CAPTION | WS_DISABLED | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME | WS_CLIPCHILDREN,
                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                NULL, hMenu, hInstance, NULL);
        }

	private:
		HWND hwnd;
	};
}