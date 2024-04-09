#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

namespace Rendering
{
    /*
        Vertex classes containing geometric data,
        geometric data: position, normal, texCoords, tangent, bitangent
    */
    class Vertex_G
    {
    public:
        Vertex_G(const DirectX::XMFLOAT3& _position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
            const DirectX::XMFLOAT3& _normal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
            const DirectX::XMFLOAT2& _texCoords = DirectX::XMFLOAT2(0.0f, 0.0f),
            const DirectX::XMFLOAT3& _tangent = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
            const DirectX::XMFLOAT3& _bitangent = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

        // Initialize vertex data(0.0f)
        void InitToDefaultValue();

        // The D3D12_INPUT_ELEMENT_DESC of this vertex class
        static const D3D12_INPUT_ELEMENT_DESC InputLayoutDesc[];

        // Get this vertex class D3D12_INPUT_LAYOUT_DESC
        static D3D12_INPUT_LAYOUT_DESC GetInputLayoutDesc();
        
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 texCoords;
        DirectX::XMFLOAT3 tangent;
        DirectX::XMFLOAT3 bitangent;
    };
}