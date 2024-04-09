#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <D3d12SDKLayers.h>
#include <d3dcompiler.h>
#include "d3dx12.h"
#include <dxgi1_4.h>

#include <vector>
#include <windows.h>
#include <wrl.h>

#include "AuxiliaryD3D.h"
#include "ConstBufferManager.h"
#include "WorldTransform.h"
#include "Exception.h"

namespace Rendering
{
	class Shader
	{
	public:
		Shader() {}

		inline Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetRootSigature()
		{
			return rootSignature;
		}

		inline D3D12_SHADER_BYTECODE GetVertexShader()
		{
			return D3D12_SHADER_BYTECODE{
				reinterpret_cast<BYTE*> (vertexShader->GetBufferPointer()),
				vertexShader->GetBufferSize() };
		}

		inline D3D12_SHADER_BYTECODE GetFragmentShader()
		{
			return D3D12_SHADER_BYTECODE{
				reinterpret_cast<BYTE*> (fragmentShader->GetBufferPointer()),
				fragmentShader->GetBufferSize() };
		}

		inline void SetVertexShader(const UString& fileName, const UString& enterPointName, const UString& target)
		{
			vertexShader = CompileShader(fileName.GetString().c_str(), nullptr, enterPointName.GetString(), target);
		}

		inline void LoadVertexShader(const UString& filename)
		{
			vertexShader = LoadBinary(filename);
		}

		inline void SetFragmentShader(const std::wstring& fileName, const std::string& enterPointName, const std::string& target)
		{
			fragmentShader = CompileShader(fileName.c_str(), nullptr, enterPointName, target);
		}

		inline void LoadFragmentShader(const std::wstring& filename)
		{
			fragmentShader = LoadBinary(filename);
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		
		Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
		Microsoft::WRL::ComPtr<ID3DBlob> fragmentShader;
	};
}