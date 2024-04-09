#pragma once

#include <map>
#include <iostream>
#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "AuxiliaryBase.h"
#include "AuxiliaryD3D.h"
#include "Vertex_GB.h"
#include "BoneInformation.h"
#include "ConstBufferType.h"
#include "ConstBufferManager.h"

namespace Rendering
{
	class Keyframe_Position
	{
	public:
		Keyframe_Position() {}

		float timePos = 0.0f;

		DirectX::XMFLOAT3 translation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	};

	class Keyframe_Scale
	{
	public:
		Keyframe_Scale() {}

		float timePos = 0.0f;

		DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	};

	class Keyframe_Rotation
	{
	public:
		Keyframe_Rotation() {}

		float timePos = 0.0f;

		DirectX::XMFLOAT4 rotationQuat = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	};


	class BoneAnimation
	{
	public:
		BoneAnimation() {}

		// Constructor: Read keyframes from aiNodeAnim and store them in this bone
		BoneAnimation(const UString& _name, int _id,
			const aiNodeAnim* channel);

		float GetStartTime()const;		// Get the start time of all keyframes
		float GetEndTime()const;		// Obtain the termination time of all keyframes

		DirectX::XMVECTOR InterpolateScale(float t)const;		// Scaling matrix obtained by interpolation at time t
		DirectX::XMVECTOR InterpolatePosition(float t)const;	// Interpolation to obtain the displacement matrix at time t
		DirectX::XMVECTOR InterpolateRotation(float t)const;	// Interpolation to obtain the rotation matrix at time t

		// Obtain the transformation matrix obtained by interpolating this skeleton at time t
		void Interpolate(float t, DirectX::XMFLOAT4X4& M)const;

		// Update transformation matrix based on time
		void Update(float animationTine);

		// bone infomation
		int id;								
		UString name;					
		DirectX::XMFLOAT4X4 localTransform;	

		// Storing a series of keyframes
		std::vector<Keyframe_Position> keyframes_position;
		std::vector<Keyframe_Scale> keyframes_scale;
		std::vector<Keyframe_Rotation> keyframes_rotation;
	};

	class AssimpNodeData
	{
	public:
		AssimpNodeData() {}

	 	static void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
		{
			if (src == nullptr)
			{
				Exception::ThrowException(LEVEL_Error, "Node is nul");
			}

			dest.name = src->mName.C_Str();
			dest.transformation = ConvertAssimpMatrixToDirectX(
				src->mTransformation);
			dest.childrenCount = src->mNumChildren;

			dest.children.resize(src->mNumChildren);
			for (int i = 0; i < src->mNumChildren; ++i)
			{
				AssimpNodeData newData;
				ReadHeirarchyData(newData, src->mChildren[i]);
				dest.children[i] = newData;
			}
		}

		DirectX::XMFLOAT4X4 transformation;		// Transformation matrix of nodes
		UString name;						// node name
		int childrenCount;						
		std::vector<AssimpNodeData> children;	// Array of child nodes of nodes
	};

	// Animation class - contains multiple bone animations
	class Animation
	{
	public:
		Animation() = default;

		/* 
			Constructor: Read information from animation files.
			Parameters: Animation file address, corresponding model bone map, 
			corresponding model bone number, and reading the index animation in the file.
		*/
		Animation(const UString& animationPath,
			std::map<UString, BoneInfo>& boneInfoMap, int& boneCount, int index);

		Animation(const aiScene* scene, std::map<UString, BoneInfo>& boneInfoMap, int& boneCount, int index);

		// Reading Bones in Animation
		void ReadMissingBones(const aiAnimation* animation,
			std::map<UString, BoneInfo>& boneInfoMap,
			int& boneCount);

		// Read scene nodes to Assymp data structure
		void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);

		// Return animation of a bone with a certain name
		BoneAnimation* FindBone(const UString& name);

		UString name;
		float m_duration;								
		int m_ticksPerSecond;							
		std::vector<BoneAnimation> m_boneAnimations;	
		AssimpNodeData m_rootNode;						
		std::map<UString, BoneInfo> m_boneInfoMap;
	};


	class Animator
	{
	public:
		Animator() = default;

		Animator(Animation* animation);

		// Update the final transformation matrix of the animation with time intervals as the parameter
		void UpdateAnimation(float dt, bool isDeleTime = true);

		void PlayAnimation(Animation* pAnimation);

		void StopPlayAnimation();

		void SetDebugMode(bool isDebug, float range0 = -1, float range1 = -1);

		/*
			Calculate the final transformation matrix of this node node and all its child nodes,
			with the node's parent matrix being parentTransform
		*/ 
		void CalculateBoneTransform(
			const AssimpNodeData* node,
			DirectX::XMFLOAT4X4 parentTransform);

		FinalTransforms finalTransforms;						// Final transformation matrix
		Animation* m_CurrentAnimation = nullptr;							
		float m_CurrentTime;									
		float m_DeltaTime;		

		float timeSpace = 1;

		bool debug = false;
		float timeRange[2];
	};
}