#pragma once

#include <iostream>
#include <windows.h>
#include <fstream>
#include <wrl.h>

#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <D3d12SDKLayers.h>
#include <d3dcompiler.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "d3dx12.h"
#include "Exception.h"

namespace Rendering
{
#ifndef MATH_PI
#define MATH_PI 3.1415926535
#endif // !MATH_PI

    inline void Transpose(DirectX::XMFLOAT4X4& res, DirectX::XMFLOAT4X4 value)
    {
        DirectX::XMStoreFloat4x4(&res,
            DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&value)));
    }

    inline CD3DX12_GPU_DESCRIPTOR_HANDLE GetIndexHandle(int index, UINT cbvSrvUavDescriptorSize,
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap)
    {
        CD3DX12_GPU_DESCRIPTOR_HANDLE tex(srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
        return tex.Offset(index, cbvSrvUavDescriptorSize);
    }

    inline DirectX::XMFLOAT4 ConvertAiQuaternionToXMFLOAT4(const aiQuaternion& quaternion)
    {
        return  DirectX::XMFLOAT4(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
    }

    inline DirectX::XMFLOAT4 ConvertAiColorToXMFLOAT4(const aiColor4D& aiVec)
    {
        return  DirectX::XMFLOAT4(aiVec.r, aiVec.g, aiVec.b, aiVec.a);
    }

    inline DirectX::XMFLOAT3 ConvertAiVectorToXMFLOAT3(const aiVector3D& aiVec)
    {
        return  DirectX::XMFLOAT3(aiVec.x, aiVec.y, aiVec.z);
    }

    inline DirectX::XMFLOAT4X4 ConvertAssimpMatrixToDirectX(const aiMatrix4x4& assimpMatrix)
    {
        DirectX::XMFLOAT4X4 directXMatrix;

        // Assimp uses column main order, while DirectX uses row main order, so transpose matrices are required 
        for (uint32_t i = 0; i < 4; ++i)
        {
            for (uint32_t j = 0; j < 4; ++j)
            {
                directXMatrix(j, i) = assimpMatrix[i][j];
            }
        }
        return directXMatrix;
    }


    // Create an initialized buffer stored in the default heap
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
        ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
        const void* initData,                                 
        UINT64 byteSize,                                      
        Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

    // Calculate the minimum constant buffer size that meets memory requirements
    UINT CalcConstantBufferByteSize(UINT byteSize);

    template <typename T>
    class UploadBuffer
    {
    public:
        UploadBuffer(ID3D12Device* device, UINT elementCount, bool _isCostantBuffer) : isCostantBuffer(_isCostantBuffer)
        {
            if (isCostantBuffer)
                elementByteSize = CalcConstantBufferByteSize(sizeof(T)); 
            else
                elementByteSize = sizeof(T);

            Exception::CheckResult(device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(elementByteSize * elementCount),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&uploadBuffer)));

            Exception::CheckResult(uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&data)));
        }

        inline D3D12_GPU_VIRTUAL_ADDRESS GetIndexBufferAddress(int index)
        {
            if (isCostantBuffer)
                return uploadBuffer->GetGPUVirtualAddress() + index * CalcConstantBufferByteSize(sizeof(T));
            else
                return uploadBuffer->GetGPUVirtualAddress() + index * sizeof(T);
        }

        // Disable copy construction and assignment operators
        UploadBuffer(const UploadBuffer& rhs) = delete;
        UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
        ~UploadBuffer()
        {
            if (uploadBuffer != nullptr)
                uploadBuffer->Unmap(0, nullptr);

            data = nullptr;
        }

        inline ID3D12Resource* Resource()const
        {
            return uploadBuffer.Get();
        }

        // Update the value of the elementIndex element in the upload buffer to elementData
        inline void CopyData(int elementIndex, const T& elementData)
        {
            memcpy(&data[elementIndex * elementByteSize], &elementData, sizeof(T));
        }

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
        BYTE* data = nullptr;
        UINT elementByteSize = 0;     // Constant buffer size for individual elements
        bool isCostantBuffer = false; 
    };

    // Compile the shader file and return the compiled bytecode result
    Microsoft::WRL::ComPtr<ID3DBlob>
        CompileShader(
            const UString& filename,
            const D3D_SHADER_MACRO* defines,
            const UString& entrypoint,
            const UString& target);

    // Load the compiled shader. cso bytecode file into the application
    Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(const UString& filename);

    DirectX::XMFLOAT4X4 Identity4X4();

    inline DirectX::XMFLOAT4X4 MatrXMatr(DirectX::XMFLOAT4X4 matr1, DirectX::XMFLOAT4X4 matr2)
    {
        DirectX::XMMATRIX value = DirectX::XMLoadFloat4x4(&matr1) * DirectX::XMLoadFloat4x4(&matr2);
        DirectX::XMFLOAT4X4 res;
        DirectX::XMStoreFloat4x4(&res, value);
        return res;
    }

    inline DirectX::XMFLOAT4X4 MatrXMatrX(DirectX::XMFLOAT4X4 matr1, DirectX::XMFLOAT4X4 matr2, DirectX::XMFLOAT4X4 matr3)
    {
        DirectX::XMMATRIX value = DirectX::XMLoadFloat4x4(&matr1) * DirectX::XMLoadFloat4x4(&matr2);
        value = value * DirectX::XMLoadFloat4x4(&matr3);
        DirectX::XMFLOAT4X4 res;
        DirectX::XMStoreFloat4x4(&res, value);
        return res;
    }

    inline DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX M)
    {
        DirectX::XMMATRIX A = M;
        A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

        DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
        return DirectX::XMMatrixTranspose(XMMatrixInverse(&det, A));
    }

    inline DirectX::XMFLOAT4X4 InverseTransposeT(DirectX::XMFLOAT4X4 M)
    {
        DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(&M);
        DirectX::XMMATRIX invTransM = InverseTranspose(m);
        DirectX::XMFLOAT4X4 res;
        DirectX::XMStoreFloat4x4(&res, invTransM);
        return res;
    }

    class ModelInfo
    {
    public:
        float position[3] = { 0.0f, 0.0f, 0.0f };
        float rotate[3] = { 0.0f, 0.0f, 0.0f };
        float scale[3] = { 1.0f, 1.0f, 1.0f };

        std::vector<std::string> animationName;
        int animationIndex = 0;
        float animationSpeed = 1.0f;

        bool zhankai = true;

        float timeRange[2] = { -1,-1 };
    };
}