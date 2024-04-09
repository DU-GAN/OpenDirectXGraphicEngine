#include "SeparateWindow.h"

namespace ViewApplication
{
    void SeparateWindow::BuildMainWindow(HWND& mainHwnd, HINSTANCE hInstance, TCHAR* wndClassName, TCHAR* appName, HBRUSH hBrush)
    {
        WNDCLASS wndclass;

        wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wndclass.lpfnWndProc = mainWndProc;
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

    void SeparateWindow::BuildToolWindow(HWND& childHwnd, HWND& parentHwnd, HINSTANCE hInstance, TCHAR* wndClassName, TCHAR* appName, HBRUSH hBrush)
    {
        WNDCLASS wndclass;

        wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wndclass.lpfnWndProc = toolWndProc;
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

        childHwnd = CreateWindow(wndClassName, appName,
            WS_CHILD,
            0, 0, 0, 0,
            parentHwnd, NULL, hInstance, NULL);
    }
    void SeparateWindow::BuildSceneWindow(HWND& childHwnd, HWND& parentHwnd, HINSTANCE hInstance, TCHAR* wndClassName, TCHAR* appName, HBRUSH hBrush)
    {
        WNDCLASS wndclass;

        wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wndclass.lpfnWndProc = sceneWndProc;
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

        childHwnd = CreateWindow(wndClassName, appName,
            WS_CHILD,
            0, 0, 0, 0,
            parentHwnd, NULL, hInstance, NULL);
    }
    void SeparateWindow::BuildRenderWindow(HWND& childHwnd, HWND& parentHwnd, HINSTANCE hInstance, TCHAR* wndClassName, TCHAR* appName, HBRUSH hBrush)
    {
        WNDCLASS wndclass;

        wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wndclass.lpfnWndProc = renderWndProc;
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

        childHwnd = CreateWindow(wndClassName, appName,
            WS_CHILD,
            0, 0, 0, 0,
            parentHwnd, NULL, hInstance, NULL);
    }
    void SeparateWindow::BuildNatureWindow(HWND& childHwnd, HWND& parentHwnd, HINSTANCE hInstance, TCHAR* wndClassName, TCHAR* appName, HBRUSH hBrush)
    {
        WNDCLASS wndclass;

        wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wndclass.lpfnWndProc = natureWndProc;
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

        childHwnd = CreateWindow(wndClassName, appName,
            WS_CHILD,
            0, 0, 0, 0,
            parentHwnd, NULL, hInstance, NULL);
    }

    void SeparateWindow::Build(HINSTANCE hInstance, TCHAR* appName)
    {
        windowVector.resize(5);
        rectVector.resize(5);

        COLORREF mainColor = RGB(95, 138, 149);
        COLORREF toolColor = RGB(255, 255, 255);
        COLORREF sceneColor = RGB(255, 255, 255);
        COLORREF renderColor = RGB(255, 255, 255);
        COLORREF natureColor = RGB(255, 255, 255);

        HBRUSH mainHBrush = CreateSolidBrush(mainColor);
        HBRUSH toolHBrush = CreateSolidBrush(toolColor);
        HBRUSH sceneHBrush = CreateSolidBrush(sceneColor);
        HBRUSH renderHBrush = CreateSolidBrush(renderColor);
        HBRUSH natureHBrush = CreateSolidBrush(natureColor);

        BuildMainWindow(windowVector[0], hInstance, mainWndClassName, appName, mainHBrush);
        BuildToolWindow(windowVector[1], windowVector[0], hInstance, toolWndClassName, appName, toolHBrush);
        BuildSceneWindow(windowVector[2], windowVector[0], hInstance, sceneWndClassName, appName, sceneHBrush);
        BuildRenderWindow(windowVector[3], windowVector[0], hInstance, renderWndClassName, appName, renderHBrush);
        BuildNatureWindow(windowVector[4], windowVector[0], hInstance, natureWndClassName, appName, natureHBrush);

        ScaleSetWorkSpaceS();
    }

    void SeparateWindow::SetWindowRect(int index)
    {
        MoveWindow(windowVector[index], rectVector[index].left, rectVector[index].top,
            rectVector[index].right - rectVector[index].left, rectVector[index].bottom - rectVector[index].top, true);
    }

    void SeparateWindow::ScaleSetWorkSpace(int index)
    {
        RECT rcParent = rectVector[0];

        if (index == 1)
        {
            double data[] = { 0,rcParent.bottom * vScale1,rcParent.right * hScale, rcParent.bottom - rcParent.bottom * vScale1 };
            data[0] += dx0;
            data[1] += (dy1 / 2);
            data[2] -= (dx1 / 2 + dx0);
            data[3] -= (dy0 + dy1 / 2);

            RECT _rect = { data[0],data[1],data[2],data[3] };
            rectVector[index] = _rect;
        }
        else if (index == 2)
        {
            double data[] = { rcParent.right * hScale,0,rcParent.right - rcParent.right * hScale,rcParent.bottom * vScale2 };
            data[0] += (dx1 / 2);
            data[1] += dy0;
            data[2] -= (dx0 + dx1 / 2);
            data[3] -= (dy1 / 2 + dy0);

            RECT _rect = { data[0],data[1],data[2],data[3] };
            rectVector[index] = _rect;
        }
        else if (index == 3)
        {
            double data[] = { 0,0,rcParent.right * hScale, rcParent.bottom * vScale1 };
            data[0] += dx0;
            data[1] += dy0;
            data[2] -= (dx1 / 2 + dx0);
            data[3] -= (dy1 / 2 + dy0);

            RECT _rect = { data[0],data[1],data[2],data[3] };
            rectVector[index] = _rect;
        }
        else if (index == 4)
        {
            double data[] = { rcParent.right * hScale,rcParent.bottom * vScale2,rcParent.right - rcParent.right * hScale,rcParent.bottom - rcParent.bottom * vScale2 };
            data[0] += (dx1 / 2);
            data[1] += (dy1 / 2);
            data[2] -= (dx0 + dx1 / 2);
            data[3] -= (dy0 + dy1 / 2);

            RECT _rect = { data[0],data[1],data[2],data[3] };
            rectVector[index] = _rect;
        }

        rectVector[index].bottom += rectVector[index].top;
        rectVector[index].right += rectVector[index].left;
    }

    void SeparateWindow::ScaleSetWorkSpaceS()
    {
        GetClientRect(windowVector[0], &rectVector[0]);     

        ScaleSetWorkSpace(1);
        ScaleSetWorkSpace(2);
        ScaleSetWorkSpace(3);
        ScaleSetWorkSpace(4);

        SetWindowRect(3);
        SetWindowRect(1);
        SetWindowRect(2);
        SetWindowRect(4);

        ShowWindow(windowVector[3], SW_SHOW);           
        ShowWindow(windowVector[1], SW_SHOW);
        ShowWindow(windowVector[2], SW_SHOW);
        ShowWindow(windowVector[4], SW_SHOW);
    }

    void SeparateWindow::ResizeWorkSpace(int index, LPARAM lParam)
    {
        POINT point = { 0,0 };
        ClientToScreen(windowVector[0], &point);

        if (index == 3)
        {
            RECT rect = *(RECT*)lParam;
            rect.left -= point.x; rect.top -= point.y;
            rect.right -= point.x; rect.bottom -= point.y;

            int dy = rect.bottom - rectVector[index].bottom;
            int dx = rect.right - rectVector[index].right;
            if (dx == 0 && dy == 0)
                return;

            rectVector[index] = rect;

            rectVector[1].top += dy;
            rectVector[1].right += dx;

            rectVector[2].left += dx;
            rectVector[4].left += dx;
        }
        else if (index == 2)
        {
            RECT rect = *(RECT*)lParam;
            rect.left -= point.x; rect.top -= point.y;
            rect.right -= point.x; rect.bottom -= point.y;

            int dy = (rect.bottom - rect.top) - (rectVector[index].bottom - rectVector[index].top);
            int dx = (rect.right - rect.left) - (rectVector[index].right - rectVector[index].left);
            if (dx == 0 && dy == 0)
                return;

            rectVector[index] = rect;

            rectVector[3].right -= dx;
            rectVector[1].right -= dx;

            rectVector[4].left += dx;
            rectVector[4].top += dy;
        }

        // ���¼������
        RECT rcChild = rectVector[index];
        rcChild.right -= rcChild.left;
        rcChild.bottom -= rcChild.top;

        RECT rcParent;
        GetClientRect(windowVector[0], &rcParent);

        if (index == 3) // render
        {
            RECT rect = rectVector[index];

            if (rect.right != rcChild.right)
            {
                hScale = (double)rcChild.right / rcParent.right;
                hScale = Rendering::Clamp(hScale, 0.0, 1.0);
            }

            if (rect.bottom != rcChild.bottom)
            {
                vScale1 = (double)rcChild.bottom / rcParent.bottom;
                vScale1 = Rendering::Clamp(vScale1, 0.0, 1.0);
            }
        }
        else if (index == 2)  // scene
        {
            RECT rect = rectVector[index];

            if (rect.right != rcChild.right)
            {
                hScale = 1.0 - (double)rcChild.right / rcParent.right;
                hScale = Rendering::Clamp(hScale, 0.0, 1.0);
            }

            if (rect.bottom != rcChild.bottom)
            {
                vScale2 = (double)rcChild.bottom / rcParent.bottom;
                vScale2 = Rendering::Clamp(vScale2, 0.0, 1.0);
            }
        }
        else if (index == 1)  // tool
        {
            RECT rect = rectVector[index];

            if (rect.right != rcChild.right)
            {
                hScale = (double)rcChild.right / rcParent.right;
                hScale = Rendering::Clamp(hScale, 0.0, 1.0);
            }

            if (rect.bottom != rcChild.bottom)
            {
                vScale1 = 1 - (double)rcChild.bottom / rcParent.bottom;
                vScale1 = Rendering::Clamp(vScale1, 0.0, 1.0);
            }
        }
        else if (index == 4)  // nature
        {
            RECT rect = rectVector[index];

            if (rect.right != rcChild.right)
            {
                hScale = 1.0 - (double)rcChild.right / rcParent.right;
                hScale = Rendering::Clamp(hScale, 0.0, 1.0);
            }

            if (rect.bottom != rcChild.bottom)
            {
                vScale2 = 1 - (double)rcChild.bottom / rcParent.bottom;
                vScale2 = Rendering::Clamp(vScale2, 0.0, 1.0);
            }
        }

        SetWindowRect(3); 
        SetWindowRect(1);
        SetWindowRect(2);
        SetWindowRect(4);

        ShowWindow(windowVector[3], SW_SHOW);   
        ShowWindow(windowVector[1], SW_SHOW);
        ShowWindow(windowVector[2], SW_SHOW);
        ShowWindow(windowVector[4], SW_SHOW);
    }
}