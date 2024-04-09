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
    LRESULT CALLBACK natureWndProc(HWND, UINT, WPARAM, LPARAM);
}