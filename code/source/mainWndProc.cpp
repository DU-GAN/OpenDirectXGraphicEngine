#include "mainWndProc.h"

namespace ViewApplication
{
    // When the main window menu is clicked
    void MainOnCommand(HWND hwnd, WPARAM wParam)
    {
        if (LOWORD(wParam) == ID_IMPORTMODEL)
        {
            IFileOpenDialog* pFileOpen;
            HRESULT hr = CoCreateInstance(
                CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                IID_IFileOpenDialog, (void**)&pFileOpen);

            if (SUCCEEDED(hr))
            {
                DWORD dwFlags;
                hr = pFileOpen->GetOptions(&dwFlags);
                hr = pFileOpen->SetOptions(dwFlags | FOS_ALLOWMULTISELECT);

                COMDLG_FILTERSPEC fileType[] =
                {
                    {L"GLTF2.0-分离式",L"*.gltf*"},
                    {L"FBX-分离式",L"*.fbx*"}
                };
                hr = pFileOpen->SetFileTypes(ARRAYSIZE(fileType), fileType);
                hr = pFileOpen->SetFileTypeIndex(1);

                hr = pFileOpen->Show(hwnd);
                if (SUCCEEDED(hr))
                {
                    IShellItem* pItem;
                    hr = pFileOpen->GetResult(&pItem);
                    if (SUCCEEDED(hr))
                    {
                        PWSTR pszFilePath;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                        if (SUCCEEDED(hr))
                        {
                            Application::GetApplication()->GetGraphicEngine().LoadModel(pszFilePath);
                            CoTaskMemFree(pszFilePath);
                        }
                        pItem->Release();
                    }
                }
                pFileOpen->Release();
            }
            return;
        }
        else if (LOWORD(wParam))
        {

        }
    }

    inline void MainOnKeyDown(WPARAM wParam)
    {

    }

    inline void MainOnMouseMove(WPARAM wParam, LPARAM lParam)
    {

    }

    LRESULT CALLBACK mainWndProc(HWND hwnd, UINT message,
        WPARAM wParam, LPARAM lParam)
    {
        GraphicEngine& ge = Application::GetApplication()->GetGraphicEngine();

        switch (message)
        {
        case WM_CREATE:
            break;
        case WM_ACTIVATE:
            GraphicEngineProc(&ge, message, wParam, lParam);
            return 0;
        case WM_ENTERSIZEMOVE:
            GraphicEngineProc(&ge, message, wParam, lParam);
            return 0;
        case WM_EXITSIZEMOVE:
            GraphicEngineProc(&ge, message, wParam, lParam);
            return 0;
        case WM_KEYDOWN:
            GraphicEngineProc(&ge, message, wParam, lParam);
            return 0;
        case WM_SIZE:
            Application::GetApplication()->GetSeparateWindow()->ScaleSetWorkSpaceS();
            return 0;
        case WM_COMMAND:
            MainOnCommand(hwnd, wParam);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_GETMINMAXINFO:
            ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 500;
            ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 500;
            return 0;
        default:
            break;
        }
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}