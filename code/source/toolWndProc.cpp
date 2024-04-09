#include "toolWndProc.h"

namespace ViewApplication
{
    LRESULT CALLBACK toolWndProc(HWND hwnd, UINT message,
        WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_CREATE:
            OnCreate(hwnd);
            break;
        case WM_SIZING:
            OnSizing(1, wParam, lParam);
            return 0;
        case WM_NCHITTEST:
            return OnNCHITTEST(1, lParam);
        default:
            break;
        }
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}