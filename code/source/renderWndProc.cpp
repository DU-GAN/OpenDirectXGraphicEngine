#include "renderWndProc.h"

namespace ViewApplication
{
    LRESULT CALLBACK renderWndProc(HWND hwnd, UINT message,
        WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
        {
            return true;
        }

        Application* application = Application::GetApplication();
        GraphicEngine& ge = application->GetGraphicEngine();

        switch (message)
        {
        case WM_SIZE:
            GraphicEngineProc(&ge, message, wParam, lParam);
            return 0;
        case WM_MOUSEMOVE:
            GraphicEngineProc(&ge, message, wParam, lParam);
            return 0;
        case WM_MOUSEWHEEL:
            GraphicEngineProc(&ge, message, wParam, lParam);
            return 0;
        case WM_CREATE:
            OnCreate(hwnd);
            GraphicEngineProc(&ge, message, wParam, lParam);
            return 0;
        case WM_SIZING:
            OnSizing(3, wParam, lParam);
            return 0;
        case WM_NCHITTEST:
            return OnNCHITTEST(3, lParam);
        case WM_GETMINMAXINFO:
            ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;  
            ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
            return 0;
        default:
            break;
        }
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}