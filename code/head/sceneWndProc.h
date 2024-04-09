#pragma once

#include <windows.h>
#include <comdef.h>
#include <ShObjIdl.h>

#include <vector>
#include <iostream>
#include <fstream>

#include "windowPublic.h"

namespace ViewApplication
{
    LRESULT CALLBACK sceneWndProc(HWND, UINT, WPARAM, LPARAM);
}