#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <string>

#include "Exception.h"
#include "AuxiliaryD3D.h"

namespace Rendering
{
    class GeometryDesc;

    // Record the vertex and index data resources uploaded to GPU.
    class GeometryResource
    {
    public:
        GeometryResource();

        GeometryResource(const std::vector<GeometryDesc>& _geoDescs);

        ~GeometryResource();

        // The GeometryDesc array must be set before uploading vertex and index data.
        void SetGeoDescs(const std::vector<GeometryDesc>& _geoDescs);

        /*
            This function uploads data described by multiple GeometryDescs to the GPU and fills in their fields such as vbv and ibv.
            After the function is called, the command queue should be refreshed before the upload heap space can be released.
        */
        void UploadVIDataToGPU(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

        // Only when refreshing the command queue can the upload heap space be released
        void FreeUploadAndNewSpace();

        // Release resources uploaded to GPU
        void Release();

        Microsoft::WRL::ComPtr<ID3D12Resource> GetVertexResourceOfGPU();

        Microsoft::WRL::ComPtr<ID3D12Resource> GetIndexResourceOfGPU();

        /*
            The grid stores continuous vertex and index data, while the model stores multiple grids.
            Before rendering the model, obtain the GeometryDesc array corresponding to multiple grids,
            set it to GeometryResource, and then upload it to GPU. When drawing, obtain information such as vbv and ibv from GeometryResource.
        */
        const std::vector<GeometryDesc>& GetGeoDescs()const;

    private:
        bool isUpload = false;

        /*
            The vertices and index memory described by GeometryDesc must be continuous,
            so it usually corresponds to the mesh. Most models contain many grids,
            so they can be represented using a GeometryDesc array.
        */
        std::vector<GeometryDesc> geoDescs;

        void* vertexData = nullptr;
        UINT64 vertexBuffersize = 0;

        Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferGPU = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferUploader = nullptr;

        void* indexData = nullptr;
        UINT64 indexBuffersize = 0;

        Microsoft::WRL::ComPtr<ID3D12Resource> indexBufferGPU = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Resource> indexBufferUploader = nullptr;
    };
}