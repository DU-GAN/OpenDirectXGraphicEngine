#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <string>

#include "AuxiliaryD3D.h"
#include "DDSTextureLoader.h"
#include "UString.h"

namespace Rendering
{
    /*
        This class is responsible for uploading textures to the GPU and building descriptor stacks
    */
    class TextureResourceWrap
    {
    public:
        TextureResourceWrap() :isUpload(false), isFreeUploadHeap(true), isBuildDescriptorHeap(false) {}

        TextureResourceWrap(const std::vector<UString> _textFilePaths)
            :textFilePaths(_textFilePaths), isUpload(false), isFreeUploadHeap(true), isBuildDescriptorHeap(false) {}

        ~TextureResourceWrap()
        {
        }

        // Set texture file paths
        void SetTextureFilePaths(const std::vector<UString>& _textFilePaths);

        // Upload textures to GPU 
        void UploadToGPU(ID3D12Device* device, ID3D12GraphicsCommandList* commmandList);

        // Free upload stack space
        void FreeUploadSpace();

        // Free All resource of GPU
        void FreeResource();

        // Create SRV descriptor stack
        void CreateSrvResourceView(ID3D12Device* device, UINT cbvSrvUavDescriptorsize,
            int minMipLevels = 0, int maxMipLevels = -1);

        // Free SRV descriptor stack
        void FreeDescriptorHeap();

        // Free All Space of CPU And GPU
        void Resert();

        // Get SRV descriptor stack
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& GetSrvDescriptorHeap();

    private:
        bool isUpload;   
        bool isFreeUploadHeap;
        bool isBuildDescriptorHeap;

        std::vector<UString> textFilePaths;
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> resources;
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> uploadHeaps;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
    };
}