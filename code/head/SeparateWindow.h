#pragma once
// ��ͷ�ļ�������WIN32���ڵ���������

#include <windows.h>
#include <comdef.h>
#include <ShObjIdl.h>

#include <vector>
#include <iostream>
#include <fstream>

#include "mainWndProc.h"
#include "natureWndProc.h"
#include "renderWndProc.h"
#include "sceneWndProc.h"
#include "toolWndProc.h"
#include "AuxiliaryBase.h"

#define WM_LoadModel 1025

namespace ViewApplication
{
    // This class separate window to 4 part
    class SeparateWindow
    {
    public:
        SeparateWindow(float _hScale = 0.8, float _vScale1 = 0.9, float _vScale2 = 0.4, float _dx0 = 15, float _dy0 = 15, float _dx1 = 10, float _dy1 = 10)
            :hScale(_hScale), vScale1(_vScale1), vScale2(_vScale2), dx0(_dx0), dy0(_dy0), dx1(_dx1), dy1(_dy1) {}

        void BuildMainWindow(HWND& mainHwnd, HINSTANCE hInstance, TCHAR* wndClassName, TCHAR* appName, HBRUSH hBrush);

        void BuildToolWindow(HWND& childHwnd, HWND& parentHwnd, HINSTANCE hInstance, TCHAR* wndClassName, TCHAR* appName, HBRUSH hBrush);
        void BuildSceneWindow(HWND& childHwnd, HWND& parentHwnd, HINSTANCE hInstance, TCHAR* wndClassName, TCHAR* appName, HBRUSH hBrush);
        void BuildRenderWindow(HWND& childHwnd, HWND& parentHwnd, HINSTANCE hInstance, TCHAR* wndClassName, TCHAR* appName, HBRUSH hBrush);
        void BuildNatureWindow(HWND& childHwnd, HWND& parentHwnd, HINSTANCE hInstance, TCHAR* wndClassName, TCHAR* appName, HBRUSH hBrush);

        void Build(HINSTANCE hInstance, TCHAR* appName);

        void SetWindowRect(int index);

        void ScaleSetWorkSpace(int index);

        void ScaleSetWorkSpaceS();

        void ResizeWorkSpace(int index, LPARAM lParam);

        TCHAR mainWndClassName[13] = TEXT("mainWndClass");
        TCHAR toolWndClassName[13] = TEXT("toolWndClass");
        TCHAR sceneWndClassName[14] = TEXT("sceneWndClass");
        TCHAR renderWndClassName[15] = TEXT("renderWndClass");
        TCHAR natureWndClassName[15] = TEXT("natureWndClass");

        std::vector<HWND> windowVector;
        std::vector<RECT> rectVector;

        double hScale, vScale1, vScale2;

        float dx0, dy0, dx1, dy1;
    };
}