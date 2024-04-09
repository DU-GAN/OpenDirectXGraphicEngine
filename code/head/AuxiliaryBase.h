#pragma once

#include <windows.h>
#include <comdef.h>
#include <ShObjIdl.h>

#include <vector>
#include <iostream>
#include <fstream>

namespace Rendering
{
    inline const char* const* ConvertVectorToStringArray(const std::vector<std::string>& vec) { 
        const size_t size = vec.size();
        const char** array = new const char* [size];

        // 将每个string的c_str()结果存储在数组中  
        for (size_t i = 0; i < size; ++i) {
            array[i] = vec[i].c_str();
        }

        return reinterpret_cast<const char* const*>(array);
    }

    inline std::wstring AnsiToWString(const std::string& str)
    {
        std::wstring result;
        int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
        LPWSTR buffer = new wchar_t[len + 1];

        MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
        buffer[len] = '\0';

        result.append(buffer);
        delete[] buffer;
        return result;
    }

    inline std::string WStringToAnsi(const std::wstring& wstr)
    {
        std::string result;
        int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
        char* buffer = new char[len + 1];

        WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
        buffer[len] = '\0';

        result.append(buffer);
        delete[] buffer;
        return result;
    }

    template<typename T>
    inline void FreeVector(std::vector<T>& vector)
    {
        vector.clear();
        std::vector<T>().swap(vector);
    }

    template<typename T>
    inline T& AddElementVector(std::vector<T>& vector)
    {
        vector.emplace_back();
        return vector.back();
    }

    template<typename T>
    inline void DeleteElementVector(std::vector<T>& vector, int index)
    {
        if (index >= vector.size())
            return;
        if (index != vector.size() - 1)
            return;

        vector.pop_back();
    }

    template<typename T>
    inline T Max(T a, T b)
    {
        return a >= b ? a : b;
    }

    template<typename T>
    inline T Min(T a, T b)
    {
        return a <= b ? a : b;
    }

    template<typename T>
    inline T Min3(T a, T b, T c)
    {
        return Min(a, Min(b, c));
    }

    template<typename T>
    inline T Max3(T a, T b, T c)
    {
        return Max(a, Max(b, c));
    }

    template<typename T>
    inline float Clamp(T value, T min, T max)
    {
        float res = Max(value, min);
        res = Min(value, max);
        return res;
    }

    class ResolutionRatio
    {
    public:
        ResolutionRatio(UINT _width = 0, UINT _height = 0, UINT _numerator = 0, UINT _denominator = 1)
            :width(_width), height(_height), numerator(_numerator), denominator(_denominator) {}

        float GetReFulshRate()const
        {
            return (1.0f * numerator) / denominator;
        }

        bool operator<(const ResolutionRatio& rr)const
        {
            if (width != rr.width)
                return width < rr.width;
            if (height != rr.height)
                return height < rr.height;
            return GetReFulshRate() < rr.GetReFulshRate();
        }

        bool operator==(const ResolutionRatio& rr)const
        {
            if (width == rr.width && height == rr.height && numerator == rr.numerator && denominator == rr.denominator)
                return true;
            else
                return false;
        }

        UINT width;
        UINT height;

        UINT numerator;
        UINT denominator;
    };

    inline void SetFullScreen(HWND hWnd)
    {
        LONG style = GetWindowLong(hWnd, GWL_STYLE);
        LONG exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

        style &= ~(WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
        exStyle |= WS_EX_TOPMOST;

        SetWindowLong(hWnd, GWL_STYLE, style);
        SetWindowLong(hWnd, GWL_EXSTYLE, exStyle);

        RECT rect;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

        SetWindowPos(hWnd, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOACTIVATE);
    }

    inline void SetWindowed(HWND hWnd)
    {
        LONG style = GetWindowLong(hWnd, GWL_STYLE);
        LONG exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

        style |= (WS_OVERLAPPEDWINDOW);
        exStyle &= ~WS_EX_TOPMOST;

        SetWindowLong(hWnd, GWL_STYLE, style);
        SetWindowLong(hWnd, GWL_EXSTYLE, exStyle);
    }

    // 打开文件对话框并返回所选文件的路径列表  
    inline std::vector<std::wstring> OpenFileDialogS(HWND hwnd) {
        std::vector<std::wstring> filePaths;

        // 创建IFileOpenDialog实例  
        IFileOpenDialog* pFileOpen = nullptr;
        HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileOpen));
        if (FAILED(hr)) {
            // 创建失败处理  
            return filePaths;
        }

        // 设置选项以允许选择多个文件，并禁止选择文件夹  
        DWORD dwFlags;
        if (SUCCEEDED(pFileOpen->GetOptions(&dwFlags))) {
            dwFlags |= FOS_ALLOWMULTISELECT | FOS_FILEMUSTEXIST;
            if (FAILED(pFileOpen->SetOptions(dwFlags))) {
                // 设置选项失败处理  
                pFileOpen->Release();
                return filePaths;
            }
        }

        // 显示对话框  
        hr = pFileOpen->Show(hwnd);
        if (hr != S_OK) {
            // 用户取消或错误处理  
            pFileOpen->Release();
            return filePaths;
        }

        // 获取结果  
        IShellItemArray* pItems = nullptr;
        if (SUCCEEDED(pFileOpen->GetResults(&pItems))) {
            DWORD count = 0;
            pItems->GetCount(&count);

            for (DWORD i = 0; i < count; ++i) {
                IShellItem* pItem = nullptr;
                if (SUCCEEDED(pItems->GetItemAt(i, &pItem))) {
                    PWSTR filePath = nullptr;
                    if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath))) {
                        filePaths.push_back(std::wstring(filePath));
                        CoTaskMemFree(filePath);
                    }
                    pItem->Release();
                }
            }
            pItems->Release();
        }

        // 清理资源  
        pFileOpen->Release();

        return filePaths;
    }
}