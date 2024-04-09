#pragma once

#include "Shader.h"

namespace Rendering
{
	// RenderPipline��ʾ��Ⱦ���߶���
	class RenderPipline
	{
	public:
		// ���캯��(��������Ⱦ������ʹ����ɫ������ĵ�ַ����Ⱦ��������)
		RenderPipline(Shader* _shader = nullptr, UString _name = "Render Pipline") :shader(_shader), name(_name)
		{
			piplineStateDesc = GetDefaultPiplineStateDesc();
			isChange = true;
		}

		inline Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetRootSignature()
		{
			return shader->GetRootSigature();
		}

		// ��ȡ��Ⱦ���ߵ�����
		inline UString GetRenderPiplineName()
		{
			return name;
		}

		// ������Ⱦ��������
		inline void setRenderPiplineName(UString _name)
		{
			name = _name;
		}

		// ������ɫ��
		inline void SetShader(Shader* _shader)
		{
			shader = _shader;
			piplineStateDesc.VS = shader->GetVertexShader();
			piplineStateDesc.PS = shader->GetFragmentShader();
			piplineStateDesc.pRootSignature = _shader->GetRootSigature().Get();
			Changing();
		}

		// �������벼������
		inline void SetInputDesc(const D3D12_INPUT_LAYOUT_DESC& inputLayoutDesc)
		{
			piplineStateDesc.InputLayout = inputLayoutDesc;
			Changing();
		}

		// ����ͼԪ����������
		inline void SetPrimiveType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
		{
			piplineStateDesc.PrimitiveTopologyType = type;
			Changing();
		}

		// ���ö������Ķ��ز������
		inline void SetSampleMask(UINT mask)
		{
			piplineStateDesc.SampleMask = mask;
			Changing();
		}

		// ���ù�դ��״̬
		inline void SetRasterizerState(D3D12_RASTERIZER_DESC desc)
		{
			piplineStateDesc.RasterizerState = desc;
			Changing();
		}

		// �������ģ����Ե�״̬
		inline void SetDepthStencilState(D3D12_DEPTH_STENCIL_DESC desc)
		{
			piplineStateDesc.DepthStencilState = desc;
			Changing();
		}

		// ����ͬʱ���õ���ȾĿ������
		inline void SetRenderNum(UINT num)
		{
			piplineStateDesc.NumRenderTargets = num;
			Changing();
		}

		// ������ȾĿ���ʽ
		void SetRTVFormats(DXGI_FORMAT* format, int length = 1)
		{
			for (int i = 0; i < length; ++i)
				piplineStateDesc.RTVFormats[i] = format[i];
			Changing();
		}

		// �������ģ�建������ʽ
		inline void SetDSVFormat(DXGI_FORMAT format)
		{
			piplineStateDesc.DSVFormat = format;
			Changing();
		}

		// ������Ⱦ���ߵĶ����
		inline void SetSampleDesc(DXGI_SAMPLE_DESC desc)
		{
			piplineStateDesc.SampleDesc = desc;
			Changing();
		}

		inline D3D12_GRAPHICS_PIPELINE_STATE_DESC GetPiplineStateDesc()
		{
			return piplineStateDesc;
		}

		// ��ȡĬ�ϵ���Ⱦ��������
		static D3D12_GRAPHICS_PIPELINE_STATE_DESC GetDefaultPiplineStateDesc()
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC resDesc;
			ZeroMemory(&resDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

			resDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			resDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	// Ĭ��ʵ����Ⱦģʽ
			resDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	// Ĭ�ϲ����ñ����޳�
			resDesc.RasterizerState.MultisampleEnable = true;

			resDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);					// ʹ��Ĭ�ϵĻ��״̬
			resDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);  // Ĭ�ϵ���Ȳ���
			resDesc.SampleMask = UINT_MAX;											// Ĭ�϶����в��������

			resDesc.NumRenderTargets = 1;
			resDesc.SampleDesc.Count = 1;
			resDesc.SampleDesc.Quality = 0;

			D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
			transparencyBlendDesc.BlendEnable = true;
			transparencyBlendDesc.LogicOpEnable = false;
			transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
			transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
			transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
			transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
			transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
			transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

			resDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;

			return resDesc;
		}

		inline void SetRootSignature(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSig)
		{
			piplineStateDesc.pRootSignature = rootSig.Get();
		}

		// ������Ⱦ���߶���(������Ⱦ���߱��޸�ʱ��Ч)
		inline void Build(ID3D12Device* device, bool enableMSAA = false, int maxSampleCount = 1)
		{
			if (!isChange)
				return;

			isChange = false;
			Exception::CheckResult(
				device->CreateGraphicsPipelineState(&piplineStateDesc, IID_PPV_ARGS(&piplineState)));

			if (enableMSAA)
			{
				D3D12_GRAPHICS_PIPELINE_STATE_DESC msaaPiplineDesc = piplineStateDesc;
				if (maxSampleCount >= 4)
				{
					msaaPiplineDesc.SampleDesc.Count = 4;
					msaaPiplineDesc.SampleDesc.Quality = 0;
					Exception::CheckResult(device->CreateGraphicsPipelineState(
						&msaaPiplineDesc, IID_PPV_ARGS(&mSAAPiplineState_4X)));
				}
				if (maxSampleCount >= 8)
				{
					msaaPiplineDesc.SampleDesc.Count = 8;
					msaaPiplineDesc.SampleDesc.Quality = 0;
					Exception::CheckResult(device->CreateGraphicsPipelineState(
						&msaaPiplineDesc, IID_PPV_ARGS(&mSAAPiplineState_8X)));
				}

			}
		}

		inline void Reset(ID3D12Device* device, bool enableMSAA = false, int maxSampleCount = 1)
		{
			Release();
			Build(device, enableMSAA, maxSampleCount);
		}

		inline void Release()
		{
			if (piplineState != nullptr)
			{
				piplineState = nullptr;
			}
			if (mSAAPiplineState_4X != nullptr)
			{
				mSAAPiplineState_4X = nullptr;
			}
			if (mSAAPiplineState_8X != nullptr)
			{
				mSAAPiplineState_8X = nullptr;
			}
		}

		// ��ȡD3D��Ⱦ���߶���
		inline Microsoft::WRL::ComPtr<ID3D12PipelineState> GetPiplineState(
			bool isMSAAPiplineState = false, int msaaSampleCount = 4)
		{
			if (isMSAAPiplineState)
			{
				if (msaaSampleCount == 4)
					return mSAAPiplineState_4X;
				else
					return mSAAPiplineState_8X;
			}
			else
				return piplineState;
		}

	private:
		// ���޸���Ⱦ�������ú󱻵���
		inline void Changing()
		{
			isChange = true;
		}

		// ��Ⱦ��������
		UString name;

		// ��Ⱦ�����Ƿ��޸�
		bool isChange;

		// ��Ⱦ����ʹ�õ���ɫ��
		Shader* shader;

		// ��Ⱦ���ߵ�����
		D3D12_GRAPHICS_PIPELINE_STATE_DESC piplineStateDesc;

		// ��Ⱦ���ߵ�3D3����
		Microsoft::WRL::ComPtr<ID3D12PipelineState> piplineState = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> mSAAPiplineState_4X = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> mSAAPiplineState_8X = nullptr;
	};
}