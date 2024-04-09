#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include "AuxiliaryD3D.h"
#include "ConstBufferManager.h"

namespace Rendering
{
	// any rendering object have world cb
	class WorldMatrix
	{
	public:
		WorldMatrix(DirectX::XMFLOAT4X4 _world = Identity4X4());

		DirectX::XMFLOAT4X4 GetWorld()const;

		void SetWorld(DirectX::XMFLOAT4X4& _world);

		WorldMatrix GetData();

	private:
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4X4 invTransWorld;
	};

	// additional info to WolrdMatrix and manage it
	class WorldTransform
	{
	public:
		WorldTransform();

		void SetScale(float x, float y, float z);

		void SetRotate(float x, float y, float z);

		void SetTranslate(float x, float y, float z);

		WorldMatrix GetWorldMatrix();

		static DirectX::XMFLOAT4X4 GetScaleMatr(DirectX::XMFLOAT3 scale);

		static DirectX::XMFLOAT4X4 GetRotateMatr(DirectX::XMFLOAT3 rotate);

		static void MatrTranslate(DirectX::XMFLOAT4X4& matr, DirectX::XMFLOAT3 translate);

		static DirectX::XMFLOAT4X4 GetWorldMatr(DirectX::XMFLOAT3 _scale,
			DirectX::XMFLOAT3 _rotate, DirectX::XMFLOAT3 _translate);

		DirectX::XMFLOAT3 scale = { 1.0f,1.0f,1.0f };
		DirectX::XMFLOAT3 rotate = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT3 transln = { 0.0f,0.0f,0.0f };
	};
}