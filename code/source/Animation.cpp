#include "Animation.h"

using namespace DirectX;

namespace Rendering
{
	BoneAnimation::BoneAnimation(const UString& _name, int _id,
		const aiNodeAnim* channel)
		: name(_name), id(_id), localTransform(Identity4X4())
	{
		// Read position keyframes
		keyframes_position.resize(channel->mNumPositionKeys);
		for (int i = 0; i < channel->mNumPositionKeys; ++i)
		{
			Keyframe_Position& key = keyframes_position[i];
			key.timePos = channel->mPositionKeys[i].mTime;
			key.translation =
				ConvertAiVectorToXMFLOAT3(
					channel->mPositionKeys[i].mValue);
		}

		// Read scaling keyframes
		keyframes_scale.resize(channel->mNumScalingKeys);
		for (int i = 0; i < channel->mNumScalingKeys; ++i)
		{
			Keyframe_Scale& key = keyframes_scale[i];
			key.timePos = channel->mScalingKeys[i].mTime;
			key.scale =
				ConvertAiVectorToXMFLOAT3(
					channel->mScalingKeys[i].mValue);
		}

		// Read rotation keyframes
		keyframes_rotation.resize(channel->mNumRotationKeys);
		for (int i = 0; i < channel->mNumRotationKeys; ++i)
		{
			Keyframe_Rotation& key = keyframes_rotation[i];
			key.timePos = channel->mRotationKeys[i].mTime;
			key.rotationQuat =
				ConvertAiQuaternionToXMFLOAT4(
					channel->mRotationKeys[i].mValue);
		}
	}

	float BoneAnimation::GetStartTime()const
	{
		if (keyframes_position.empty() || keyframes_rotation.empty() || keyframes_scale.empty())
			Exception::ThrowException(LEVEL_Error, "have frames is empty");

		return Min3(keyframes_position.front().timePos, keyframes_rotation.front().timePos, keyframes_scale.front().timePos);
	}

	float BoneAnimation::GetEndTime()const
	{
		if (keyframes_position.empty() || keyframes_rotation.empty() || keyframes_scale.empty())
			Exception::ThrowException(LEVEL_Error, "have frames is empty");

		return Max3(keyframes_position.back().timePos, keyframes_rotation.back().timePos, keyframes_scale.back().timePos);
	}

	DirectX::XMVECTOR BoneAnimation::InterpolateScale(float t)const
	{
		if (keyframes_scale.empty())
			Exception::ThrowException(LEVEL_Error, "key is empty");

		if (t <= keyframes_scale.front().timePos)
		{
			return XMLoadFloat3(&keyframes_scale.front().scale);
		}
		else if (t >= keyframes_scale.back().timePos)
		{
			return XMLoadFloat3(&keyframes_scale.back().scale);
		}
		else
		{
			// ����������ؼ�֮֡��
			for (UINT i = 0; i < keyframes_scale.size() - 1; ++i)
			{
				// �ҵ���������
				if (t >= keyframes_scale[i].timePos && t <= keyframes_scale[i + 1].timePos)
				{
					// ���㵱ǰʱ��������ռ��
					float lerpPercent = (t - keyframes_scale[i].timePos) / (keyframes_scale[i + 1].timePos - keyframes_scale[i].timePos);

					XMVECTOR s0 = XMLoadFloat3(&keyframes_scale[i].scale);
					XMVECTOR s1 = XMLoadFloat3(&keyframes_scale[i + 1].scale);

					// ����ռ�Ⱥ��������˹ؼ�֡����ֵ�����š�λ�ƺ���ת
					return XMVectorLerp(s0, s1, lerpPercent);
				}
			}
		}
	}

	DirectX::XMVECTOR BoneAnimation::InterpolatePosition(float t)const
	{
		if (keyframes_position.empty())
			Exception::ThrowException(LEVEL_Error, "key is empty");

		// �������ʱ���ڵ�һ֮֡ǰ�����һ֮֡���򷵻ض�Ӧ֡�ı任����
		if (t <= keyframes_position.front().timePos)
		{
			return XMLoadFloat3(&keyframes_position.front().translation);
		}
		else if (t >= keyframes_position.back().timePos)
		{
			return XMLoadFloat3(&keyframes_position.back().translation);
		}
		else
		{
			// ����������ؼ�֮֡��
			for (UINT i = 0; i < keyframes_position.size() - 1; ++i)
			{
				// �ҵ���������
				if (t >= keyframes_position[i].timePos && t <= keyframes_position[i + 1].timePos)
				{
					// ���㵱ǰʱ��������ռ��
					float lerpPercent = (t - keyframes_position[i].timePos) / (keyframes_position[i + 1].timePos - keyframes_position[i].timePos);

					XMVECTOR p0 = XMLoadFloat3(&keyframes_position[i].translation);
					XMVECTOR p1 = XMLoadFloat3(&keyframes_position[i + 1].translation);

					// ����ռ�Ⱥ��������˹ؼ�֡����ֵ�����š�λ�ƺ���ת
					return XMVectorLerp(p0, p1, lerpPercent);
				}
			}
		}
	}

	DirectX::XMVECTOR BoneAnimation::InterpolateRotation(float t)const
	{
		if (keyframes_rotation.empty())
			Exception::ThrowException(LEVEL_Error, "key is empty");

		// �������ʱ���ڵ�һ֮֡ǰ�����һ֮֡���򷵻ض�Ӧ֡�ı任����
		if (t <= keyframes_rotation.front().timePos)
		{
			return XMLoadFloat4(&keyframes_rotation.front().rotationQuat);
		}
		else if (t >= keyframes_rotation.back().timePos)
		{
			return XMLoadFloat4(&keyframes_rotation.back().rotationQuat);
		}
		else
		{
			for (UINT i = 0; i < keyframes_rotation.size() - 1; ++i)
			{
				// �ҵ���������
				if (t >= keyframes_rotation[i].timePos && t <= keyframes_rotation[i + 1].timePos)
				{
					// ���㵱ǰʱ��������ռ��
					float lerpPercent = (t - keyframes_rotation[i].timePos) / (keyframes_rotation[i + 1].timePos - keyframes_rotation[i].timePos);

					XMVECTOR r0 = XMLoadFloat4(&keyframes_rotation[i].rotationQuat);
					XMVECTOR r1 = XMLoadFloat4(&keyframes_rotation[i + 1].rotationQuat);

					// ����ռ�Ⱥ��������˹ؼ�֡����ֵ�����š�λ�ƺ���ת
					return XMQuaternionSlerp(r0, r1, lerpPercent);
				}
			}
		}
	}

	void BoneAnimation::Interpolate(float t, XMFLOAT4X4& M)const
	{
		XMVECTOR S = InterpolateScale(t);
		XMVECTOR P = InterpolatePosition(t);
		XMVECTOR Q = InterpolateRotation(t);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&M, XMMatrixAffineTransformation(S, zero, Q, P));
	}

	void BoneAnimation::Update(float animationTine)
	{
		Interpolate(animationTine, localTransform);
	}

	Animation::Animation(const UString& animationPath,
		std::map<UString, BoneInfo>& boneInfoMap, int& boneCount, int index)
	{
		Assimp::Importer importer;

		//	To maintain consistency, use the same options as model import
		unsigned int option =
			aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace |
			aiProcess_FlipUVs | aiProcess_FlipWindingOrder | aiProcess_MakeLeftHanded |
			aiProcess_OptimizeMeshes | aiProcess_ImproveCacheLocality | aiProcessPreset_TargetRealtime_MaxQuality;

		const aiScene* scene = importer.ReadFile(animationPath.GetString(), option);

		if (scene && scene->mRootNode && (index < scene->mNumAnimations))
		{
			Exception::ThrowException(LEVEL_Error, "Load Animation ERROR");
		}

		name = scene->mAnimations[index]->mName.C_Str();

		auto animation = scene->mAnimations[index];				// Get the index animation of the animation file
		m_duration = animation->mDuration;						// Save animation duration
		m_ticksPerSecond = animation->mTicksPerSecond;			// Save Animation Timescale

		ReadHeirarchyData(m_rootNode, scene->mRootNode);		// Read scene nodes to Assymp data structure
		ReadMissingBones(animation, boneInfoMap, boneCount);	// Reading Bones in Animation
	}

	Animation::Animation(const aiScene* scene, std::map<UString, BoneInfo>& boneInfoMap, int& boneCount, int index)
	{
		if (!scene || !scene->mRootNode || !(index < scene->mNumAnimations))
		{
			Exception::ThrowException(LEVEL_Error, "Load Animation ERROR");
		}

		name = scene->mAnimations[index]->mName.C_Str();

		auto animation = scene->mAnimations[index];				// Get the index animation of the animation file
		m_duration = animation->mDuration;						// Save animation duration
		m_ticksPerSecond = animation->mTicksPerSecond;			// Save Animation Timescale

		ReadHeirarchyData(m_rootNode, scene->mRootNode);		// Read scene nodes to Assymp data structure
		ReadMissingBones(animation, boneInfoMap, boneCount);	// Reading Bones in Animation
	}

	// Reading Bones in Animation
	void Animation::ReadMissingBones(const aiAnimation* animation,
		std::map<UString, BoneInfo>& boneInfoMap,
		int& boneCount)
	{
		int size = animation->mNumChannels;

		for (int i = 0; i < size; ++i)
		{
			auto channel = animation->mChannels[i];
			UString boneName = channel->mNodeName.data;

			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				boneInfoMap[boneName].id = boneCount;
				boneCount += 1;
			}

			m_boneAnimations.emplace_back(boneName, boneInfoMap[channel->mNodeName.data].id, channel);
		}

		m_boneInfoMap = boneInfoMap;
	}

	// Read scene nodes to Assymp data structure
	void Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
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

	// Return animation of a bone with a certain name
	BoneAnimation* Animation::FindBone(const UString& name)
	{
		auto iter = std::find_if(m_boneAnimations.begin(),
			m_boneAnimations.end(),
			[&](const BoneAnimation& boneAnim)
			{
				return boneAnim.name == name;
			});
		if (iter == m_boneAnimations.end())return nullptr;
		else return &(*iter);
	}

	Animator::Animator(Animation* animation)
	{
		m_CurrentTime = 0.0;
		m_CurrentAnimation = animation;
	}

	// Update the final transformation matrix of the animation with time intervals as the parameter
	void Animator::UpdateAnimation(float dt, bool isDeleTime)
	{
		m_DeltaTime = dt;
		float duration = m_CurrentAnimation->m_duration;
		if (m_CurrentAnimation != NULL)
		{
			if (debug)
			{
				duration = timeRange[1] - timeRange[0];
			}
			
			if (isDeleTime)
			{
				m_CurrentTime +=
					m_CurrentAnimation->m_ticksPerSecond * dt * timeSpace;
			}
			else
				m_CurrentTime = m_CurrentAnimation->m_ticksPerSecond * dt * timeSpace;
			
			m_CurrentTime = fmod(
				m_CurrentTime, duration);
			if (debug)
			{
				m_CurrentTime += timeRange[0];
			}

			CalculateBoneTransform(
				&m_CurrentAnimation->m_rootNode, Identity4X4());

			for (int i = 0; i < MAX_BONE_NUM_OFMODEL; ++i)
			{
				XMStoreFloat4x4(&finalTransforms.boneTransforms[i], XMMatrixTranspose(XMLoadFloat4x4(&finalTransforms.boneTransforms[i])));
			}
		}
	}

	void Animator::PlayAnimation(Animation* pAnimation)
	{
		m_CurrentAnimation = pAnimation;
		m_CurrentTime = 0.0f;
	}

	void Animator::StopPlayAnimation()
	{
		m_CurrentAnimation = nullptr;
		for (int i = 0; i < MAX_BONE_NUM_OFMODEL; ++i)
		{
			finalTransforms.boneTransforms[i] = Identity4X4();
		}
	}

	void Animator::SetDebugMode(bool isDebug, float range0, float range1)
	{
		debug = isDebug;
		timeRange[0] = range0;
		timeRange[1] = range1;
	}

	/*
		Calculate the final transformation matrix of this node node and all its child nodes,
		with the node's parent matrix being parentTransform
	*/
	void Animator::CalculateBoneTransform(
		const AssimpNodeData* node,
		DirectX::XMFLOAT4X4 parentTransform)
	{
		UString nodeName = node->name;
		DirectX::XMFLOAT4X4 nodeTransform =
			node->transformation;

		BoneAnimation* boneAnim =
			m_CurrentAnimation->FindBone(nodeName);

		if (boneAnim)
		{
			boneAnim->Update(m_CurrentTime);
			nodeTransform = boneAnim->localTransform;
		}

		DirectX::XMFLOAT4X4 globalTransformation =
			MatrXMatr(nodeTransform, parentTransform);

		auto boneInfoMap = m_CurrentAnimation->m_boneInfoMap;
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			DirectX::XMFLOAT4X4 offset = boneInfoMap[nodeName].offset;
			finalTransforms.boneTransforms[index] = MatrXMatr(offset, globalTransformation);
		}

		// Calculate the final transformation matrix for all child nodes
		for (int i = 0; i < node->childrenCount; ++i)
		{
			CalculateBoneTransform(&node->children[i], globalTransformation);
		}
	}
}