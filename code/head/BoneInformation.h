#pragma once

#include <map>
#include <string>
#include <vector>

namespace Rendering
{
	// Bone information class 
	class BoneInfo
	{
	public:
		int id;
		DirectX::XMFLOAT4X4 offset;
	};

	class BoneInformation
	{
	public:
		BoneInformation() {}

		int boneCount = 0;

		std::map<UString, BoneInfo> boneMap;
	};
}