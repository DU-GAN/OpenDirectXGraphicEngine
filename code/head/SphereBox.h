#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <D3d12SDKLayers.h>
#include <d3dcompiler.h>

#include "AuxiliaryD3D.h"

namespace Rendering
{
	inline float Abs(float v)
	{
		return v >= 0 ? v : -1 * v;
	}

#define epsilon 1e-6f

	inline bool Equal(float f1, float f2)
	{
		return Abs(f1 - f2) <= epsilon;
	}

	class Vector3f
	{
	public:
		Vector3f(float x = 0.0f, float y = 0.0f, float z = 0.0f)
		{
			data = DirectX::XMFLOAT3(x, y, z);
		}

		Vector3f(DirectX::XMFLOAT3 _data)
		{
			data = _data;
		}

		Vector3f(DirectX::XMVECTOR ve)
		{
			DirectX::XMStoreFloat3(&data, ve);
		}

		DirectX::XMVECTOR Get()const
		{
			return DirectX::XMLoadFloat3(&data);
		}

		float& operator[](int index)
		{
			switch (index)
			{
			case 0:
				return data.x;
			case 1:
				return data.y;
			case 2:
				return data.z;
			default:
				break;
			}
		}

		const float& operator[](int index)const
		{
			switch (index)
			{
			case 0:
				return data.x;
			case 1:
				return data.y;
			case 2:
				return data.z;
			default:
				break;
			}
		}

		Vector3f operator+(const Vector3f& v)const
		{
			return DirectX::XMVectorAdd(Get(), v.Get());
		}

		Vector3f operator-(const Vector3f& v)const
		{
			return DirectX::XMVectorSubtract(Get(), v.Get());
		}

		Vector3f operator*(const float& k)const
		{
			return DirectX::XMVectorScale(Get(), k);
		}

		float Length()const
		{
			float x = data.x;
			float y = data.y;
			float z = data.z;
			return sqrt(x * x + y * y + z * z);
		}

		float LengthTow()const
		{
			float x = data.x;
			float y = data.y;
			float z = data.z;
			return x * x + y * y + z * z;
		}

		static float Dot(const Vector3f& v1, const Vector3f& v2)
		{
			return
				DirectX::XMVectorGetX(
					DirectX::XMVector3Dot(v1.Get(), v2.Get()));
		}

		static Vector3f Cross(const Vector3f& v1, const Vector3f& v2)
		{
			DirectX::XMVECTOR cross
				= DirectX::XMVector3Cross(
					v1.Get(), v2.Get());
			return cross;
		}

	private:
		DirectX::XMFLOAT3 data;
	};

	typedef Vector3f Point3f;

	class SphereBox
	{
	public:
		SphereBox(
			Point3f _center = Point3f(0.0f, 0.0f, 0.0f),
			float _radius = 0.0f) :center(_center), radius(_radius) {}

		SphereBox(const Point3f& p1, const Point3f& p2)
		{
			center = ((p1 + p2) * 0.5f);
			radius = ((p1 - p2) * 0.5f).Length();
		}

		SphereBox(const Point3f& p1, const Point3f& p2, const Point3f& p3)
		{
			Vector3f e0 = p2 - p1, e1 = p3 - p1;
			float a = Vector3f::Dot(e0, e0);
			float b = Vector3f::Dot(e0, e1);
			float c = Vector3f::Dot(e1, e1);
			float d = a * c - b * b;

			if (Equal(d, 0))
			{
				Vector3f e2 = p3 - p2;
				if (e0.Length() >= e1.Length() && e0.Length() >= e2.Length())
				{
					SphereBox(p1, p2);
				}
				else if (
					e1.Length() >= e0.Length() && e1.Length() >= e2.Length())
				{
					SphereBox(p1, p3);
				}
				else if (
					e2.Length() >= e0.Length() && e2.Length() >= e1.Length())
				{
					SphereBox(p2, p3);
				}
				return;
			}

			float s = (a - b) * c / (2 * d);
			float t = (c - b) * a / (2 * d);

			center = p1 + e0 * s + e1 * t;
			radius = (p1 - center).Length();
		}

		SphereBox(const Point3f& p1, const Point3f& p2,
			const Point3f& p3, const Point3f& p4)
		{
			Vector3f v1 = p2 - p1, v2 = p3 - p1, v3 = p4 - p1;
			float V = Vector3f::Dot(v1, Vector3f::Cross(v2, v3));

			if (Equal(V, 0))
			{
				SphereBox s1(p1, p2, p3);
				SphereBox s2(p1, p2, p4);
				SphereBox s3(p1, p3, p4);
				SphereBox s4(p2, p3, p4);

				if (s1.radius >= s2.radius
					&& s1.radius >= s3.radius && s1.radius >= s4.radius)
				{
					(*this) = s1;
				}
				else if (s2.radius >= s1.radius
					&& s2.radius >= s3.radius && s2.radius >= s4.radius)
				{
					(*this) = s2;
				}
				else if (s3.radius >= s2.radius
					&& s3.radius >= s1.radius && s3.radius >= s4.radius)
				{
					(*this) = s3;
				}
				else if (s4.radius >= s1.radius
					&& s4.radius >= s2.radius && s4.radius >= s3.radius)
				{
					(*this) = s4;
				}
				return;
			}

			V *= 2.0;
			float L1 = v1.Length(), L2 = v2.Length(), L3 = v3.Length();
			center[0] = (p1[0] + ((v2[1] * v3[2] - v3[1] * v2[2]) * L1 - (v1[1] * v3[2] - v3[1] * v1[2]) * L2 + (v1[1] * v2[2] - v2[1] * v1[2]) * L3) / V);
			center[1] = (p1[1] + (-(v2[0] * v3[2] - v3[0] * v2[2]) * L1 + (v1[0] * v3[2] - v3[0] * v1[2]) * L2 - (v1[0] * v2[2] - v2[0] * v1[2]) * L3) / V);
			center[2] = (p1[2] + ((v2[0] * v3[1] - v3[0] * v2[1]) * L1 - (v1[0] * v3[1] - v3[0] * v1[1]) * L2 + (v1[0] * v2[1] - v2[0] * v1[1]) * L3) / V);
			radius = (center - p1).Length();
		}

		static bool isOut(const Point3f& point, const SphereBox& sphere)
		{
			float dis = (point - sphere.center).LengthTow();
			return dis <= (sphere.radius * sphere.radius);
		}

		struct Int2
		{
			Int2(int _a, int _b) :a(_a), b(_b) {}

			union
			{
				int a, b;
				int data[2];
			};
		};

		static SphereBox WelzlSphere(
			Point3f* points, int numPts)
		{
			SphereBox sphereBox;
			for (int i = 0; i < numPts; ++i)
			{
				if (!isOut(points[i], sphereBox))
				{
					sphereBox = SphereBox(points[i]);
					for (int j = 0; j < i; ++j)
					{
						if (!isOut(points[j], sphereBox))
						{
							sphereBox = SphereBox(points[i], points[j]);
							for (int k = 0; k < j; ++k)
							{
								if (!isOut(points[k], sphereBox))
								{
									sphereBox = SphereBox(points[i], points[j], points[k]);
									for (int n = 0; n < k; ++n)
									{
										if (!isOut(points[n], sphereBox))
										{
											sphereBox = SphereBox(points[i], points[j], points[k], points[n]);
										}
									}
								}
							}
						}
					}
				}
			}

			return sphereBox;
		}

		static SphereBox GetSphereBox(Point3f* points, int numPts)
		{
			return WelzlSphere(points, numPts);
		}

		static bool Test(
			const SphereBox& s1, const SphereBox& s2)
		{
			Vector3f d = s1.center - s2.center;
			float dist2 = Vector3f::Dot(d, d);

			float radiusSum = s1.radius + s2.radius;
			return dist2 <= radiusSum * radiusSum;
		}

		Point3f center;
		float radius;
	};
}