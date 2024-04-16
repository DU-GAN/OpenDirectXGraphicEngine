#include "WorldTransform.h"

namespace Rendering
{
	WorldMatrix::WorldMatrix(DirectX::XMFLOAT4X4 _world)
	{
		SetWorld(_world);
	}

	DirectX::XMFLOAT4X4 WorldMatrix::GetWorld()const
	{
		return world;
	}

	void WorldMatrix::SetWorld(DirectX::XMFLOAT4X4& _world)
	{
		world = _world;
		invTransWorld = InverseTransposeT(world);
	}

	WorldMatrix WorldMatrix::GetData()
	{
		WorldMatrix res;
		DirectX::XMFLOAT4X4 matr;
		Transpose(matr, world);
		res.SetWorld(matr);

		return res;
	}

	WorldTransform::WorldTransform() {}

	void WorldTransform::SetScale(float x, float y, float z)
	{
		scale.x = x;
		scale.y = y;
		scale.z = z;
	}

	void WorldTransform::SetRotate(float x, float y, float z)
	{
		rotate.x = x;
		rotate.y = y;
		rotate.z = z;
	}

	void WorldTransform::SetTranslate(float x, float y, float z)
	{
		transln.x = x;
		transln.y = y;
		transln.z = z;
	}

	WorldMatrix WorldTransform::GetWorldMatrix()
	{
		return GetWorldMatr(scale, rotate, transln);
	}

	DirectX::XMFLOAT4X4 WorldTransform::GetScaleMatr(DirectX::XMFLOAT3 scale)
	{
		return
		{
			scale.x,0.0f,0.0f,0.0f,
			0.0f,scale.y,0.0f,0.0f,
			0.0f,0.0f,scale.z,0.0f,
			0.0f,0.0f,0.0f,1.0f
		};
	}

	DirectX::XMFLOAT4X4 WorldTransform::GetRotateMatr(DirectX::XMFLOAT3 rotate)
	{
		float cosX = cos(rotate.x);	float sinX = sin(rotate.x);
		float cosY = cos(rotate.y);	float sinY = sin(rotate.y);
		float cosZ = cos(rotate.z);	float sinZ = sin(rotate.z);
		DirectX::XMFLOAT4X4 rx =
		{
			1.0f,0.0f,0.0f,0.0f,
			0.0f,cosX,sinX,0.0f,
			0.0f,-sinX,cosX,0.0f,
			0.0f,0.0f,0.0f,1.0f
		};
		DirectX::XMFLOAT4X4 ry =
		{
			cosY,0.0f,-sinY,0.0f,
			0.0f,1.0f,0.0f,0.0f,
			sinY,0.0f,cosY,0.0f,
			0.0f,0.0f,0.0f,1.0f
		};
		DirectX::XMFLOAT4X4 rz =
		{
			cosZ,sinZ,0.0f,0.0f,
			-sinZ,cosZ,0.0f,0.0f,
			0.0f,0.0f,1.0f,0.0f,
			0.0f,0.0f,0.0f,1.0f
		};
		return MatrXMatrX(rx, ry, rz);
	}

	void WorldTransform::MatrTranslate(DirectX::XMFLOAT4X4& matr, DirectX::XMFLOAT3 translate)
	{
		matr._41 += translate.x;
		matr._42 += translate.y;
		matr._43 += translate.z;
	}

	DirectX::XMFLOAT4X4 WorldTransform::GetWorldMatr(DirectX::XMFLOAT3 _scale,
		DirectX::XMFLOAT3 _rotate, DirectX::XMFLOAT3 _translate)
	{
		DirectX::XMFLOAT4X4 resWorld = MatrXMatr(GetScaleMatr(_scale), GetRotateMatr(_rotate));
		MatrTranslate(resWorld, _translate);
		return resWorld;
	}
}