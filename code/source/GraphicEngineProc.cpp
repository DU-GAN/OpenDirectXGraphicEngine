#include "GraphicEngineProc.h"

namespace ViewApplication
{
	void GraphicEngineProc(GraphicEngine* ge, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (!ge->IsBuild())
			return;

		static int mousePosx = -1, mousePosy = -1;
		int intValue1 = 0, intValue2 = 0, intValue3 = 0, intValue4 = 0;
		float floatValue1 = 0.0f;

		switch (message)
		{
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
				ge->SetPaused(true);
			else
				ge->SetPaused(false);
			return;
		case WM_SIZE:
			ge->SetScreenSize(LOWORD(lParam), HIWORD(lParam));
			return;
		case WM_ENTERSIZEMOVE:
			ge->SetPaused(true);
			return;
		case WM_EXITSIZEMOVE:
			ge->SetPaused(false);
			return;
		case WM_KEYDOWN:
			if (wParam == 0x57)			// W
				ge->GetCamera().Walk(1.0f);
			else if (wParam == 0x41)	// A
				ge->GetCamera().Strafe(-1.0f);
			else if (wParam == 0x44)	// D
				ge->GetCamera().Strafe(1.0f);
			else if (wParam == 0x53)	// S
				ge->GetCamera().Walk(-1.0f);
			else if (wParam == VK_F1)
				ge->SetMsaaState(!ge->GetMsaaState());
			else if (wParam == VK_F2)
				ge->SetMSAASampleCount(4);
			else if (wParam == VK_F3)
				ge->SetMSAASampleCount(8);
			ge->GetCamera().UpdateViewMatrix();
			return;
		case WM_MOUSEMOVE:
			if (mousePosx < 0)
			{
				mousePosx = LOWORD(lParam);
				mousePosy = HIWORD(lParam);
			}
			else
			{
				if (wParam == MK_MBUTTON)
				{
					float dx = DirectX::XMConvertToRadians(
						0.25f * static_cast<float>(LOWORD(lParam) - mousePosx));
					float dy = DirectX::XMConvertToRadians(
						0.25f * static_cast<float>(HIWORD(lParam) - mousePosy));

					ge->GetCamera().Pitch(dy / 2);
					ge->GetCamera().RotateY(dx / 2);
				}
				mousePosx = LOWORD(lParam);
				mousePosy = HIWORD(lParam);
			}
			return;
		case WM_MOUSEWHEEL:
			intValue1 = GET_WHEEL_DELTA_WPARAM(wParam);
			floatValue1 = ge->GetCamera().GetFovY();
			ge->GetCamera().SetFovY(floatValue1 - (intValue1) / 1200.0);
			return;
		default:
			return;
		}
	}
}