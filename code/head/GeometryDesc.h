#pragma once

#include <d3d12.h>
#include <vector>

#include "GeometryResource.h"

namespace Rendering
{
    // Describe the continuous vertex and index data that needs to be uploaded to the GPU
    class GeometryDesc
    {
    public:
        GeometryDesc() {}

        // below function help me to use this class, but you can also directly use the properties of this class
        UINT GetIndexCount()const;
        UINT GetStartIndexLocation()const;
        UINT GetBaseVertexLocation()const;

        const void* GetVertexData()const;
        const void* GetIndexData()const;

        UINT GetVertexTypesize()const;
        UINT GetIndexTypesize()const;

        UINT64 GetVertexDatasize()const;
        UINT64 GetIndexDatasize()const;

        D3D12_VERTEX_BUFFER_VIEW GetVertexView()const;
        D3D12_INDEX_BUFFER_VIEW GetIndexView()const;

        void SetIndexCount(UINT _indexCount);
        void SetStartIndexLocation(UINT _startIndexLocation);
        void SetBaseVertexLocation(UINT _baseVertexLocation);

        void SetVertexData(const void* _vertexData, UINT _vertexsize, UINT64 _vertexDatasize);
        void SetIndexData(const void* _indexData, UINT _indexsize, UINT64 _indexDatasize);

        void SetVBV(const D3D12_VERTEX_BUFFER_VIEW& _vbv);
        void SetIBV(const D3D12_INDEX_BUFFER_VIEW& _ibv);

        const GeometryResource* GetVIB()const;
        void SetVIB(const GeometryResource* _PResource);

    private:
        const void* vertexData = nullptr;       // Vertex data memory first address
        UINT vertexsize;                        // The size of a single vertex variable
        UINT64 vertexBuffersize;                // The entire vertex memory size
        D3D12_VERTEX_BUFFER_VIEW vbv;

        const void* indexData = nullptr;        // Index data memory first address
        UINT indexsize;
        UINT64 indexBuffersize;
        D3D12_INDEX_BUFFER_VIEW ibv;

        UINT indexCount;
        UINT startIndexLocation;
        UINT baseVertexLocation;

        /*
           GeometryResource uploads vertex and index data from multiple GeometryDesc descriptions.
           After uploading, GeometryDesc will be filled in with fields such as
           vbv, ibv, startIndexLocation, and baseVertexLocation as parameters for calling D3D rendering.
           After uploading, GeometryDesc will save the address of GeometryResource, which can be indexed to GPU resources.
        */
        const GeometryResource* PResource = nullptr;
    };
}