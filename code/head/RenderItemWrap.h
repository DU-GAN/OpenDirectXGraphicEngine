#pragma once

#include "Material.h"
#include "GeometryDesc.h"
#include "WorldTransform.h"
#include "TextureResource.h"
#include "Animation.h"
#include "RenderItem.h"
#include "RenderPipeline.h"
#include "ConstBufferType.h"
#include "ConstBufferManager.h"

namespace Rendering
{
	class GPUMemory;

	class RenderItemWrap
	{
	public:
		RenderItemWrap() {}

		void SetRenderPipline(RenderPipline* _renderPiplineP)
		{
			renderPiplineP = _renderPiplineP;
		}

		void SetPrimitiveType(D3D12_PRIMITIVE_TOPOLOGY _primitiveType)
		{
			primitiveType = _primitiveType;
		}

		void SetPRDC(ID3D12GraphicsCommandList* commandList, FrameIndex frameIndex, 
			ConstBufferManager<PassCB, PassCB>& passCB, bool enableMSAA = false)
		{
			commandList->SetPipelineState(renderPiplineP->GetPiplineState(enableMSAA).Get());
			commandList->SetGraphicsRootSignature(renderPiplineP->GetRootSignature().Get());

			if (suportTexture)
			{
				ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap.Get() };
				commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
			}

			if (!suportAnimation)
			{
				commandList->SetGraphicsRootConstantBufferView(2, passCB.GetCBAddress(frameIndex));
			}
			else
			{
				commandList->SetGraphicsRootConstantBufferView(2, finalTransformsP->GetCBAddress(frameIndex));
				commandList->SetGraphicsRootConstantBufferView(3, passCB.GetCBAddress(frameIndex));
			}
		}

		void DrawWrap(ID3D12GraphicsCommandList* commandList, FrameIndex frameIndex,
			ConstBufferManager<PassCB, PassCB>& passCB, UINT cbvSrvUavDescriptorSize, bool enableMSAA = false)
		{
			SetPRDC(commandList, frameIndex, passCB, enableMSAA);

			D3D12_GPU_VIRTUAL_ADDRESS worldCBA;
			D3D12_GPU_VIRTUAL_ADDRESS materialCBA;

			for (int i = 0; i < renderItems.size(); ++i)
			{
				worldCBA = worldCB->GetCBAddress(frameIndex, i);
				materialCBA = materialCBP->GetCBAddress(renderItems[i].materialIndex);

				renderItems[i].DrawItem(
					commandList, cbvSrvUavDescriptorSize, primitiveType,
					worldCBA, srvDescriptorHeap, materialCBA, suportAnimation);
			}
		}

		std::vector<RenderItem> renderItems;
		ConstBufferManager<WorldMatrix, WorldTransform>* worldCB;

		bool suportTexture = false;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;

		bool suportAnimation = false;
		std::vector<Animation>* animationsP;
		ConstBufferManager<FinalTransforms, FinalTransforms>* finalTransformsP;

		ConstBufferManager<PBR_Material_Data, PBR_Material_Data>* materialCBP;

		RenderPipline* renderPiplineP;
		D3D12_PRIMITIVE_TOPOLOGY primitiveType;

		bool isVisible = true;

		GPUMemory* memory;
	public:
		WorldTransform& GetWorldTransform()
		{
			return worldTransform;
		}

		void UpdateWorldCB(FrameIndex frameIndex)
		{
			WorldMatrix wm;
			DirectX::XMFLOAT4X4 matr;
			for (int i = 0; i < renderItems.size(); ++i)
			{
				matr = MatrXMatr(renderItems[i].transform, worldTransform.GetWorldMatrix().GetWorld());;
				wm.SetWorld(matr);
				worldCB->Change(frameIndex, wm.GetData(), i);
			}
		}

		std::vector<Animation>* GetAnimationsP()
		{
			return animationsP;
		}

		void PlayAnimation(Animation* animation)
		{
			animator.PlayAnimation(animation);
		}

		void StopPlayAnimaiton(FrameIndex frameIndex)
		{
			animator.StopPlayAnimation();
			FinalTransforms& fl = animator.finalTransforms;
			finalTransformsP->Change(frameIndex, fl);
		}

		void UpdateAnimation(FrameIndex frameIndex, float dt, bool isDeleTime = true)
		{
			if (animator.m_CurrentAnimation != nullptr)
			{
				animator.UpdateAnimation(dt, isDeleTime);
				FinalTransforms& fl = animator.finalTransforms;
				finalTransformsP->Change(frameIndex, fl);
			}
		}

		WorldTransform worldTransform;
		Animator animator;
	};
}