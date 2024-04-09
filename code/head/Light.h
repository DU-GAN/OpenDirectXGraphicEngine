#pragma once

#include <DirectXMath.h>
#include <DirectXPackedVector.h>

namespace Rendering
{
	enum LightType
	{
		ParallelLight,
		PointLight,
		SpotLight
	};

#ifndef NUM_MAX_LIGHT
#define NUM_MAX_LIGHT 30
#endif 
	class Light
	{
	public:
		DirectX::XMFLOAT3 radiant;
		int type;
		DirectX::XMFLOAT3 pos;
		float falloffEnd;
	};

	class LightManager
	{
	public:
		Light* GetLights()
		{
			return lights;
		}

		void SetNumLight(int numLight)
		{
			lightNum = numLight;
		}

		int GetNumLight()
		{
			return lightNum;
		}
	private:
		int lightNum = 0;
		Light lights[NUM_MAX_LIGHT];
	};
}
