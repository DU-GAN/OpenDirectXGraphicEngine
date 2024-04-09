#include "windowPublic.h"

namespace ViewApplication
{
    void D3DMsgProcAndResultWndProc(UINT message, WPARAM wParam, LPARAM lParam)
    {
        GraphicEngineProc(&Application::GetApplication()->GetGraphicEngine(), message, wParam, lParam);
    }

    void OnCreate(HWND childHwnd)
    {
        SetWindowLong(childHwnd, GWL_STYLE, GetWindowLong(childHwnd, GWL_STYLE) &
            (~(WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)));
    }

    void OnSizing(int index, WPARAM wParam, LPARAM lParam)
    {
        if (wParam == WMSZ_TOPLEFT || wParam == WMSZ_TOPRIGHT || wParam == WMSZ_BOTTOMLEFT || wParam == WMSZ_BOTTOMRIGHT)
            return;
        Application::GetApplication()->GetSeparateWindow()->ResizeWorkSpace(index, lParam);
    }

    LRESULT OnNCHITTEST(int index, LPARAM lParam)
    {
        POINT point = { LOWORD(lParam),HIWORD(lParam) };
        ScreenToClient(Application::GetApplication()->GetSeparateWindow()->windowVector[index], &point);

        int disC = 30;
        if (index == 3)
        {
            if (abs(point.y - Application::GetApplication()->GetSeparateWindow()->rectVector[index].bottom) < disC)
            {
                return HTBOTTOM;
            }
            else if (abs(point.x - Application::GetApplication()->GetSeparateWindow()->rectVector[index].right) < disC)
            {
                return HTRIGHT;
            }
            else
                return HTCLIENT;
        }
        else if (index == 1)
        {
            return HTCLIENT;
        }
        else if (index == 2)
        {
            if (abs(point.y - Application::GetApplication()->GetSeparateWindow()->rectVector[index].bottom) < disC)
            {
                return HTBOTTOM;
            }
            return HTCLIENT;
        }
        else
        {
            return HTCLIENT;
        }
    }
}