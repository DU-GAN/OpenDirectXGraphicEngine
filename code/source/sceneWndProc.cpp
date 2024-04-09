#include "sceneWndProc.h"

namespace ViewApplication
{
    LRESULT CALLBACK sceneWndProc(HWND hwnd, UINT message,
        WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_CREATE:
            OnCreate(hwnd);
            return 0;
        case WM_SIZING:
            OnSizing(2, wParam, lParam);
            return 0;
        case WM_NCHITTEST:
            return OnNCHITTEST(2, lParam);
        default:
            break;
        }
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}