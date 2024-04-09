#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include "AuxiliaryD3D.h"
#include "RenderResouce.h"

namespace Rendering
{
	class Camera
	{
	public:

		Camera();
		~Camera();

		// Get/Set world camera position.
		DirectX::XMVECTOR GetPosition()const;
		DirectX::XMFLOAT3 GetPosition3f()const;
		void SetPosition(float x, float y, float z);
		void SetPosition(const DirectX::XMFLOAT3& v);

		// Obtain the vector of the camera
		DirectX::XMVECTOR GetRight()const;
		DirectX::XMFLOAT3 GetRight3f()const;
		DirectX::XMVECTOR GetUp()const;
		DirectX::XMFLOAT3 GetUp3f()const;
		DirectX::XMVECTOR GetLook()const;
		DirectX::XMFLOAT3 GetLook3f()const;

		float GetNearZ()const;
		float GetFarZ()const;
		float GetAspect()const;
		float GetFovY()const;
		float GetFovX()const;

		float GetNearWindowWidth()const;
		float GetNearWindowHeight()const;
		float GetFarWindowWidth()const;
		float GetFarWindowHeight()const;

		void SetLens(float fovY, float aspect, float zn, float zf);

		void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
		void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

		DirectX::XMMATRIX GetView();
		DirectX::XMMATRIX GetProj()const;

		DirectX::XMFLOAT4X4 GetView4x4f();
		DirectX::XMFLOAT4X4 GetProj4x4f()const;

		void Strafe(float d);
		void Walk(float d);

		void Pitch(float angle);
		void RotateY(float angle);

		void UpdateViewMatrix();

		inline void SetFovY(float _mFovY)
		{
			mFovY = _mFovY;
			mFovY = Max(0.1f, mFovY);
			mFovY = Min(3.0f, mFovY);
			SetLens(mFovY, mAspect, mNearZ, mFarZ);
		}

	private:

		DirectX::XMFLOAT3 mPosition = { 0.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 mRight = { 1.0f, 0.0f, 0.0f };
		DirectX::XMFLOAT3 mUp = { 0.0f, 1.0f, 0.0f };
		DirectX::XMFLOAT3 mLook = { 0.0f, 0.0f, 1.0f };

		float mNearZ = 0.0f;
		float mFarZ = 0.0f;
		float mAspect = 0.0f;
		float mFovY = 0.0f;
		float mNearWindowHeight = 0.0f;
		float mFarWindowHeight = 0.0f;

		bool mViewDirty = true;

		DirectX::XMFLOAT4X4 mView = Identity4X4();
		DirectX::XMFLOAT4X4 mProj = Identity4X4();
	};
}