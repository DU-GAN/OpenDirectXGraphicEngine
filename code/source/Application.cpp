#include "Application.h"

namespace ViewApplication
{
    Application* Application::application = nullptr;

    void Application::Build(HINSTANCE _hInstance)
    {
        hInstance = _hInstance;

        if (windowIsOnly)
        {
            onlyWindow = new OnlyWindow();
            onlyWindow->Build(_hInstance, appName);
            graphicEngine.Build(hInstance, onlyWindow->GetHWND());
        }
        else
        {
            separWindow = new SeparateWindow();
            separWindow->Build(hInstance, appName);
            graphicEngine.Build(hInstance, separWindow->windowVector[3]);
        }
    }

    void Application::ShowAndEnableWindow(int nShowCmd)
    {
        if (!showAndEnableWindow)
        {
            showAndEnableWindow = true;
            ShowWindow(GetWindowHWND(), nShowCmd);
            UpdateWindow(GetWindowHWND());

            EnableWindow(GetWindowHWND(), true);
        }
    }

    LRESULT Application::Run(int nShowCmd)
    {
        SetProcessDPIAware();

        MSG msg = { 0 };

        timer.Reset();

        while (msg.message != WM_QUIT)
        {
            timer.Tick();

            if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                if (!renderPaused)
                {
                    graphicEngine.Draw();
                    graphicEngine.UpdateEngine();
                        
                    ShowAndEnableWindow(nShowCmd);
                }
                else
                {
                    Sleep(100);
                }
            }
        }

        return msg.wParam;
    }

    HWND Application::GetWindowHWND()
    {
        if(!onlyWindow && !separWindow->windowVector[0])
            Exception::ThrowException(LEVEL_Prompt,L"not build");

        if (windowIsOnly)
        {
            return onlyWindow->GetHWND();
        }
        else
            return separWindow->windowVector[0];
    }

    Application* Application::GetApplication(bool _windowIsOnly)
    {
        if (application != nullptr)
        {
            if ((_windowIsOnly && application->onlyWindow == nullptr)
                || (!_windowIsOnly && application->separWindow == nullptr))
            {
                Exception::ThrowException(
                    LEVEL_Error, "The parameter type is inconsistent with the build time");
            }
            return application;
        }
        else
        {
            application = new Application(_windowIsOnly);
            return application;
        }
    }

    OnlyWindow* Application::GetOnlyWindow()
    {
        return onlyWindow;
    }

    SeparateWindow* Application::GetSeparateWindow()
    {
        return separWindow;
    }

    Rendering::GraphicEngine& Application::GetGraphicEngine()
    {
        return graphicEngine;
    }

    void Application::SetRenderPaused(bool _renderPaused)
    {
        if (renderPaused != _renderPaused)
        {
            renderPaused = _renderPaused;
            graphicEngine.SetPaused(renderPaused);
        }
    }

    void Application::SetShowFrameInfo(bool _showFrameInfo)
    {
        if (showFrameInfo != _showFrameInfo)
        {
            showFrameInfo = _showFrameInfo;
        }
    }

    Application::Application(bool _windowIsOnly)
    {
        windowIsOnly = _windowIsOnly;
        if (!windowIsOnly)
        {
            SetProcessDPIAware();
            CoInitialize(NULL);
        }
    }
}