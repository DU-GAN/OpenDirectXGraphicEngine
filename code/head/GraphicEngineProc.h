#pragma once

#include "D3DCore.h"

using namespace Rendering;

namespace ViewApplication
{
	// This is ViewApplication defined grahpic engine processing message logic
	void GraphicEngineProc(GraphicEngine* ge, UINT message, WPARAM wParam, LPARAM lParam);
}