#pragma once

#include "AuxiliaryD3D.h"
#include "Light.h"
#include "MacroDefine.h"

namespace Rendering
{
	class CoordinateColor
	{
	public:
		DirectX::XMFLOAT4 xColor = { 1.0f, 0.0f, 0.0f, 1.0f };
		DirectX::XMFLOAT4 zColor = { 0.0f, 0.0f, 1.0f, 1.0f };
		DirectX::XMFLOAT4 cColor = { 0.0f, 0.5f, 0.0f, 1.0f };
	};

	class PassCB
	{
	public:
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 invView;
		DirectX::XMFLOAT4X4 proj;
		DirectX::XMFLOAT4X4 invProj;
		DirectX::XMFLOAT4X4 viewProj;
		DirectX::XMFLOAT4X4 invViewProj;
		DirectX::XMFLOAT3 cameraPosW;
		int lightNum;
		float nearZ;
		float farZ;
		float totalTime;
		float deltaTime;

		Light lights[NUM_MAX_LIGHT];
		int renderType;
	};

	class FinalTransforms
	{
	public:
		FinalTransforms()
		{
			for (int i = 0; i < MAX_BONE_NUM_OFMODEL; ++i)
			{
				boneTransforms[i] = Identity4X4();
			}
		}

		DirectX::XMFLOAT4X4 boneTransforms[MAX_BONE_NUM_OFMODEL];
	};
}