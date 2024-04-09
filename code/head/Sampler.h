#pragma once

#include <d3d12.h>
#include "d3dx12.h"
#include "AuxiliaryBase.h"

namespace Rendering
{
    std::vector<CD3DX12_STATIC_SAMPLER_DESC> GetStaticSamplers(int anisLevels = 16, int minMipmapLevels = 0, float maxMipmapLevels = D3D12_FLOAT32_MAX);
}