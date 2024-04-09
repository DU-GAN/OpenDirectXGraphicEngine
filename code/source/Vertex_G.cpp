#include "Vertex_G.h"

namespace Rendering
{
    Vertex_G::Vertex_G(const DirectX::XMFLOAT3& _position,
        const DirectX::XMFLOAT3& _normal,
        const DirectX::XMFLOAT2& _texCoords,
        const DirectX::XMFLOAT3& _tangent,
        const DirectX::XMFLOAT3& _bitangent)
        : position(_position), normal(_normal), texCoords(_texCoords), tangent(_tangent), bitangent(_bitangent) {}

    void Vertex_G::InitToDefaultValue()
    {
        position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        normal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        texCoords = DirectX::XMFLOAT2(0.0f, 0.0f);
        tangent = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
        bitangent = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    }

    const D3D12_INPUT_ELEMENT_DESC Vertex_G::InputLayoutDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };

    D3D12_INPUT_LAYOUT_DESC Vertex_G::GetInputLayoutDesc()
    {
        D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
        inputLayoutDesc.pInputElementDescs = InputLayoutDesc;
        inputLayoutDesc.NumElements = 5;
        return inputLayoutDesc;
    }
}