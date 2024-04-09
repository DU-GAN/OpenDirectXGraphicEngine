#pragma once

#include <windows.h>
#include <comdef.h>
#include <ShObjIdl.h>

#include <vector>
#include <iostream>
#include <fstream>

#include "OnlyWindow.h"
#include "SeparateWindow.h"
#include "D3DCore.h"
#include "Timer.h"

using namespace Rendering;

namespace ViewApplication
{
    class OnlyWindow;
    class SeparateWindow;

    class Application
    {
    public:
        ~Application()
        {
            if (!windowIsOnly)
            {
                
            }

            delete application;
            application = nullptr;
        }

        void Build(HINSTANCE _hInstance);

        LRESULT Run(int nShowCmd);

        void ShowAndEnableWindow(int nShowCmd);

        HWND GetWindowHWND();

        static Application* GetApplication(bool _windowIsOnly = true);

        OnlyWindow* GetOnlyWindow();

        SeparateWindow* GetSeparateWindow();

        Rendering::GraphicEngine& GetGraphicEngine();

        void SetRenderPaused(bool _renderPaused);

        void SetShowFrameInfo(bool _showFrameInfo);

    private:
        Application(bool _windowIsOnly = false);

        static Application* application;    

        bool windowIsOnly;
        OnlyWindow* onlyWindow = nullptr;
        SeparateWindow* separWindow = nullptr;
        Rendering::GraphicEngine graphicEngine;

        TCHAR appName[16] = TEXT("ViewApplication");

        HINSTANCE hInstance;

        Timer timer;
        bool renderPaused = false;
        bool showFrameInfo = true;

        bool showAndEnableWindow = false;
    };
}