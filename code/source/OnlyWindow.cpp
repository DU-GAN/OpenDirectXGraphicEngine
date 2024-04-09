#include "OnlyWindow.h"

namespace ViewApplication
{
    LRESULT CALLBACK OnlyWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
        {
            return true;
        }

        Application* app = Application::GetApplication();
        LRESULT res = -24;
        static bool showGUI = false;

        switch (message)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            res = 0; break;
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE)
                app->SetRenderPaused(true);
            else
                app->SetRenderPaused(false);
            res = 0; break;
        case WM_ENTERSIZEMOVE:
            app->SetRenderPaused(true);
            res = 0; break;
        case WM_EXITSIZEMOVE:
            app->SetRenderPaused(false);
            res = 0; break;
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE)		// ESC
                PostQuitMessage(0);
            if (wParam == 0x51)             // Q
            {
                showGUI = !showGUI;
                app->GetGraphicEngine().SetShowGUI(showGUI);
            }
            res = 0; break;
        case WM_GETMINMAXINFO:
            ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 500;
            ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 500;
            res = 0; break;
        default:
            break;
        }

        GraphicEngine& ge = Application::GetApplication()->GetGraphicEngine();
        GraphicEngineProc(&ge, message, wParam, lParam);

        if (res != -24)
            return res;
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    void OnlyWindow::Build(HINSTANCE hInstance, TCHAR* appName)
    {
        WNDCLASS wndclass;
        COLORREF mainColor = RGB(255, 0, 0);

        HBRUSH hBrush = CreateSolidBrush(mainColor);

        wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wndclass.lpfnWndProc = OnlyWndProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = hInstance;
        wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndclass.hbrBackground = hBrush;
        wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName = L"Only window class";

        if (!RegisterClass(&wndclass))
        {
            MessageBox(NULL, TEXT("RigisterClass function failed:" + GetLastError()), TEXT("Error"), MB_ICONERROR);
            return;
        }

        hwnd = CreateWindow(L"Only window class", appName,
            WS_CAPTION | WS_DISABLED | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME | WS_CLIPCHILDREN,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            NULL, NULL, hInstance, NULL);
    }

    HWND OnlyWindow::GetHWND()
    {
        return hwnd;
    }
}