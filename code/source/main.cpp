#include <windows.h>

#include "Application.h"

using namespace Rendering;
using namespace ViewApplication;

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
#ifdef _DEBUG
#pragma warning(disable:4996)
    AllocConsole();
    freopen("CONOUT$", "w+t", stdout);
#endif // _DEBUG

    Application* application = Application::GetApplication();

    application->Build(hInstance);

    application->Run(nShowCmd);

    return 0;
}