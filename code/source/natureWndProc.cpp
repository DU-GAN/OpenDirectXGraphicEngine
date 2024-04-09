#include "natureWndProc.h"

namespace ViewApplication
{
    LRESULT CALLBACK natureWndProc(HWND hwnd, UINT message,
        WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_CREATE:
            OnCreate(hwnd);
            break;
        case WM_SIZING:
            OnSizing(4, wParam, lParam);
            return 0;
        case WM_NCHITTEST:
            return OnNCHITTEST(4, lParam);
        default:
            break;
        }
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}