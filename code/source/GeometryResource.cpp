#include "GeometryDesc.h"

namespace Rendering
{
    UINT GeometryDesc::GetIndexCount()const
    {
        return indexCount;
    }
    UINT GeometryDesc::GetStartIndexLocation()const
    {
        return startIndexLocation;
    }
    UINT GeometryDesc::GetBaseVertexLocation()const
    {
        return baseVertexLocation;
    }

    const void* GeometryDesc::GetVertexData()const
    {
        return vertexData;
    }
    const void* GeometryDesc::GetIndexData()const
    {
        return indexData;
    }

    UINT GeometryDesc::GetVertexTypesize()const
    {
        return vertexsize;
    }
    UINT GeometryDesc::GetIndexTypesize()const
    {
        return indexsize;
    }

    UINT64 GeometryDesc::GetVertexDatasize()const
    {
        return vertexBuffersize;
    }
    UINT64 GeometryDesc::GetIndexDatasize()const
    {
        return indexBuffersize;
    }

    D3D12_VERTEX_BUFFER_VIEW GeometryDesc::GetVertexView()const
    {
        return vbv;
    }
    D3D12_INDEX_BUFFER_VIEW GeometryDesc::GetIndexView()const
    {
        return ibv;
    }

    void GeometryDesc::SetIndexCount(UINT _indexCount)
    {
        if (_indexCount < 0)
            Exception::ThrowException(LEVEL_Warning, "index count < 0");

        indexCount = _indexCount;
    }
    void GeometryDesc::SetStartIndexLocation(UINT _startIndexLocation)
    {
        if (_startIndexLocation < 0)
            Exception::ThrowException(LEVEL_Warning, "startIndexLocation < 0");

        startIndexLocation = _startIndexLocation;
    }
    void GeometryDesc::SetBaseVertexLocation(UINT _baseVertexLocation)
    {
        if (_baseVertexLocation < 0)
            Exception::ThrowException(LEVEL_Warning, "baseVertexLocation < 0");

        baseVertexLocation = _baseVertexLocation;
    }

    void GeometryDesc::SetVertexData(const void* _vertexData, UINT _vertexsize, UINT64 _vertexDatasize)
    {
        if (_vertexData == nullptr || _vertexsize <= 0 || _vertexDatasize < 0)
            Exception::ThrowException(LEVEL_Warning, "ERROR");

        vertexData = _vertexData;
        vertexsize = _vertexsize;
        vertexBuffersize = _vertexDatasize;
    }
    void GeometryDesc::SetIndexData(const void* _indexData, UINT _indexsize, UINT64 _indexDatasize)
    {
        if (_indexData == nullptr || _indexsize < 0 || _indexDatasize < 0)
            Exception::ThrowException(LEVEL_Warning, "ERROR");

        indexData = _indexData;
        indexsize = _indexsize;
        indexBuffersize = _indexDatasize;
    }

    void GeometryDesc::SetVBV(const D3D12_VERTEX_BUFFER_VIEW& _vbv)
    {
        vbv = _vbv;
    }
    void GeometryDesc::SetIBV(const D3D12_INDEX_BUFFER_VIEW& _ibv)
    {
        ibv = _ibv;
    }

    const GeometryResource* GeometryDesc::GetVIB()const
    {
        return PResource;
    }
    void GeometryDesc::SetVIB(const GeometryResource* _PResource)
    {
        PResource = _PResource;
    }

    void GeometryResource::SetGeoDescs(const std::vector<GeometryDesc>& _geoDescs)
    {
        geoDescs.resize(_geoDescs.size());
        for (int i = 0; i < _geoDescs.size(); ++i)
        {
            geoDescs[i] = _geoDescs[i];
        }
    }

    void GeometryResource::UploadVIDataToGPU(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
    {
        if (isUpload || geoDescs.size() < 1)
            Exception::ThrowException(LEVEL_Prompt, "ERROR");

        // Calculate the index starting position and vertex starting position for each geometric description.
        UINT indexCount = 0;
        UINT vertexCount = 0;

        // The vertex and index starting point of the first geometric term are both 0.
        geoDescs[0].SetStartIndexLocation(indexCount);
        geoDescs[0].SetBaseVertexLocation(vertexCount);

        // For each subsequent geometric term description
        for (int i = 1; i < geoDescs.size(); ++i)
        {
            // Accumulate the vertices and indices of the previous geometric terms.
            indexCount += geoDescs[i - 1].GetIndexCount();
            vertexCount += (geoDescs[i - 1].GetVertexDatasize() / geoDescs[i - 1].GetVertexTypesize());

            /*
                Each geometry calculates vbv and ibv based on offset, 
                so the starting positions of vertices and indexes are both 0.
            */
            geoDescs[i].SetStartIndexLocation(0);
            geoDescs[i].SetBaseVertexLocation(0);
        }

        /*
            In order to submit discrete data to the GPU, 
            it is necessary to integrate the vertex and index data of multiple geometric terms into a continuous block of memory. 
            Record the offset of each geometric vertex and index data relative to the start of continuous memory here.
        */
        std::vector<UINT64> vertexStartPosition(geoDescs.size());
        std::vector<UINT64> indexStartPosition(geoDescs.size());

        vertexStartPosition[0] = 0;
        indexStartPosition[0] = 0;

        // Accumulate vertex and index buffer sizes
        vertexBuffersize = geoDescs[0].GetVertexDatasize();
        indexBuffersize = geoDescs[0].GetIndexDatasize();

        for (int i = 1; i < geoDescs.size(); ++i)
        {
            /*
                The starting position of each geometric vertex and index buffer is 
                equal to the sum of the sizes of the previous geometric vertex or index buffer
            */
            vertexStartPosition[i] = vertexBuffersize;
            indexStartPosition[i] = indexBuffersize;

            // Accumulate vertex and index buffer sizes
            vertexBuffersize += geoDescs[i].GetVertexDatasize();
            indexBuffersize += geoDescs[i].GetIndexDatasize();
        }

        // Apply for a buffer size that can accommodate all rendered vertex and index data
        vertexData = new char[vertexBuffersize];
        indexData = new char[indexBuffersize];

        // Copy the vertices and index data of geometric terms to the corresponding positions in the entire buffer block
        for (int i = 0; i < geoDescs.size(); ++i)
        {
            memcpy((char*)vertexData + vertexStartPosition[i], geoDescs[i].GetVertexData(), geoDescs[i].GetVertexDatasize());
            memcpy((char*)indexData + indexStartPosition[i], geoDescs[i].GetIndexData(), geoDescs[i].GetIndexDatasize());
        }

        // Create a vertex and index upload heap, and transfer an entire block of data onto it
        vertexBufferGPU = CreateDefaultBuffer(
            device, cmdList, vertexData, vertexBuffersize, vertexBufferUploader);

        indexBufferGPU = CreateDefaultBuffer(
            device, cmdList, indexData, indexBuffersize, indexBufferUploader);

        D3D12_VERTEX_BUFFER_VIEW vbv;
        D3D12_INDEX_BUFFER_VIEW ibv;

        // Fill in VBV and IBV fields for each geometric item
        for (int i = 0; i < geoDescs.size(); ++i)
        {
            vbv.BufferLocation = vertexBufferGPU->GetGPUVirtualAddress() + vertexStartPosition[i];
            vbv.StrideInBytes = geoDescs[i].GetVertexTypesize();
            vbv.SizeInBytes = geoDescs[i].GetVertexDatasize();
            geoDescs[i].SetVBV(vbv);

            ibv.BufferLocation = indexBufferGPU->GetGPUVirtualAddress() + indexStartPosition[i];
            if (geoDescs[i].GetIndexTypesize() == 2)
                ibv.Format = DXGI_FORMAT_R16_UINT;
            else if (geoDescs[i].GetIndexTypesize() == 4)
                ibv.Format = DXGI_FORMAT_R32_UINT;
            else
                Exception::ThrowException(LEVEL_Error, "index Format faild");

            ibv.SizeInBytes = geoDescs[i].GetIndexDatasize();

            geoDescs[i].SetIBV(ibv);
            geoDescs[i].SetVIB(this);   // GeoDescs can be indexed to GeometryResource
        }

        isUpload = true;
    }

    void GeometryResource::FreeUploadAndNewSpace()
    {
        if (!isUpload)
            Exception::ThrowException(LEVEL_Error, "Not upload");

        if (vertexData != nullptr)
        {
            vertexBufferUploader->Release();
            indexBufferUploader->Release();

            delete vertexData;
            delete indexData;

            vertexData = nullptr;
            indexData = nullptr;
        }
    }

    void GeometryResource::Release()
    {
        if (isUpload)
        {
            FreeUploadAndNewSpace();

            vertexBufferGPU = nullptr;
            indexBufferGPU = nullptr;
        }

        isUpload = false;
    }

    Microsoft::WRL::ComPtr<ID3D12Resource> GeometryResource::GetVertexResourceOfGPU()
    {
        return vertexBufferGPU;
    }

    Microsoft::WRL::ComPtr<ID3D12Resource> GeometryResource::GetIndexResourceOfGPU()
    {
        return indexBufferGPU;
    }

    const std::vector<GeometryDesc>& GeometryResource::GetGeoDescs()const
    {
        if (!isUpload)
            Exception::ThrowException(LEVEL_Error, "Resources not uploaded");
        return geoDescs;
    }

    GeometryResource::GeometryResource() {}

    GeometryResource::GeometryResource(const std::vector<GeometryDesc>& _geoDescs) :geoDescs(_geoDescs)
    {
        if (_geoDescs.empty())
            Exception::ThrowException(LEVEL_Warning, "Calling the destructor released GPU resources");
    }

    GeometryResource::~GeometryResource()
    {
    }
}