#include "TextureResource.h"

namespace Rendering
{
    void TextureResourceWrap::SetTextureFilePaths(const std::vector<UString>& _textFilePaths)
    {
        textFilePaths = _textFilePaths;
    }

    void TextureResourceWrap::UploadToGPU(ID3D12Device* device, ID3D12GraphicsCommandList* commmandList)
    {
        if (isUpload || textFilePaths.empty())
        {
            Exception::ThrowException(LEVEL_Warning, "Not have texure");
            if (isUpload)
            {
                FreeResource();
            }
            else
                return;
        }

        resources.resize(textFilePaths.size());
        uploadHeaps.resize(textFilePaths.size());
        for (int i = 0; i < textFilePaths.size(); ++i)
        {
            Exception::CheckResult(DirectX::CreateDDSTextureFromFile12(
                device, commmandList, textFilePaths[i].GetWstring().c_str(), resources[i], uploadHeaps[i]));
        }

        isUpload = true;
        isFreeUploadHeap = false;
    }

    void TextureResourceWrap::FreeUploadSpace()
    {
        if (!isUpload || isFreeUploadHeap || !isBuildDescriptorHeap)
        {
            Exception::ThrowException(LEVEL_Warning, "Faild");
            return;
        }
        for (int i = 0; i < uploadHeaps.size(); ++i)
        {
            uploadHeaps[i] = nullptr;
        }
        isFreeUploadHeap = true;
    }

    void TextureResourceWrap::FreeResource()
    {
        if (!isUpload || !isFreeUploadHeap)
        {
            Exception::ThrowException(LEVEL_Warning, "Faild");
            if (!isFreeUploadHeap)
            {
                FreeUploadSpace();
            }
            else if (!isUpload)
                return;
        }
        if (isBuildDescriptorHeap)
        {
            srvDescriptorHeap = nullptr;
        }
        for (int i = 0; i < uploadHeaps.size(); ++i)
        {
            resources[i] = nullptr;
        }
        isUpload = false;
        isBuildDescriptorHeap = false;
        isFreeUploadHeap = true;
    }

    void TextureResourceWrap::CreateSrvResourceView(ID3D12Device* device, UINT cbvSrvUavDescriptorSize,
        int minMipLevels, int maxMipLevels)
    {
        if (!isUpload)
        {
            Exception::ThrowException(LEVEL_Warning, "Faild");
            return;
        }

        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.NumDescriptors = textFilePaths.size();
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        Exception::CheckResult(device->CreateDescriptorHeap(
            &srvHeapDesc, IID_PPV_ARGS(&srvDescriptorHeap)));

        CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(
            srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = minMipLevels;
        srvDesc.Texture2D.MipLevels = maxMipLevels;

        for (int i = 0; i < textFilePaths.size(); ++i)
        {
            srvDesc.Format = resources[i]->GetDesc().Format;
            srvDesc.Texture2D.MipLevels = resources[i]->GetDesc().MipLevels;
            device->CreateShaderResourceView(resources[i].Get(), &srvDesc, hDescriptor);

            hDescriptor.Offset(1, cbvSrvUavDescriptorSize);
        }

        isBuildDescriptorHeap = true;
    }

    // ���ͷ���������
    void TextureResourceWrap::FreeDescriptorHeap()
    {
        if (!isBuildDescriptorHeap)
        {
            Exception::ThrowException(LEVEL_Warning, "Faild");
            return;
        }
        srvDescriptorHeap->Release();
        isBuildDescriptorHeap = false;
    }

    // ����������Դ�������ͷ�����GPU��CPU������Դ
    void TextureResourceWrap::Resert()
    {
        if (!isFreeUploadHeap)
        {
            FreeUploadSpace();
        }
        if (isUpload)
        {
            FreeResource();
        }
        if (isBuildDescriptorHeap)
        {
            FreeDescriptorHeap();
        }
        textFilePaths.resize(0);
        resources.resize(0);
        uploadHeaps.resize(0);
    }

    // ��ȡ��������
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& TextureResourceWrap::GetSrvDescriptorHeap()
    {
        if (!isBuildDescriptorHeap)
        {
            Exception::ThrowException(LEVEL_Warning, "Faild");
        }
        return srvDescriptorHeap;
    }
}