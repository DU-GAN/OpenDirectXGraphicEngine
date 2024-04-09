#include "Sampler.h"

namespace Rendering
{
    // ��ȡ��̬���������飬�������������Բ�������(1-16)��������С�����Mipmap�㼶(0-D3D12_FLOAT32_MAX)
    std::vector<CD3DX12_STATIC_SAMPLER_DESC> GetStaticSamplers(int anisLevels, int minMipmapLevels, float maxMipmapLevels)
    {
        std::vector<CD3DX12_STATIC_SAMPLER_DESC> staticSamplerDesc(6);

        CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
            0,
            D3D12_FILTER_ANISOTROPIC,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            0,
            anisLevels,
            D3D12_COMPARISON_FUNC_ALWAYS,
            D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
            minMipmapLevels,
            maxMipmapLevels);
        staticSamplerDesc[0] = anisotropicWrap;

        CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
            1,
            D3D12_FILTER_ANISOTROPIC,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            0,
            anisLevels,
            D3D12_COMPARISON_FUNC_ALWAYS,
            D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
            minMipmapLevels,
            maxMipmapLevels);
        staticSamplerDesc[1] = anisotropicClamp;

        CD3DX12_STATIC_SAMPLER_DESC linearWrap(
            2,
            D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            0,
            0,
            D3D12_COMPARISON_FUNC_ALWAYS,
            D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
            minMipmapLevels,
            maxMipmapLevels);
        staticSamplerDesc[2] = linearWrap;

        CD3DX12_STATIC_SAMPLER_DESC linearClamp(
            3,
            D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            0,
            0,
            D3D12_COMPARISON_FUNC_ALWAYS,
            D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
            minMipmapLevels,
            maxMipmapLevels);
        staticSamplerDesc[3] = linearClamp;

        CD3DX12_STATIC_SAMPLER_DESC linearPointWrap(
            4,
            D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            0,
            0,
            D3D12_COMPARISON_FUNC_ALWAYS,
            D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
            minMipmapLevels,
            maxMipmapLevels);
        staticSamplerDesc[4] = linearPointWrap;

        CD3DX12_STATIC_SAMPLER_DESC linearPointClamp(
            5,
            D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
            0,
            0,
            D3D12_COMPARISON_FUNC_ALWAYS,
            D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
            minMipmapLevels,
            maxMipmapLevels);
        staticSamplerDesc[5] = linearPointClamp;

        return staticSamplerDesc;
    }
}