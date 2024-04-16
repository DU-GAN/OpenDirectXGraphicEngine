#include "PBR_Auxiliary.hlsl"

cbuffer worldMatr : register(b0)
{
    float4x4 cWorld;
    float4x4 cInvTransWorld;
};

cbuffer pbrMaterial : register(b1)
{
    PBR_Material material;
};

cbuffer commonData : register(b3)
{
    float4x4 cView;
    float4x4 cInvView;
    float4x4 cProj;
    float4x4 cInvProj;
    float4x4 cViewProj;
    float4x4 cInvViewProj;
    float3 cCameraPosW;
    int cLightNum;
    float cNearZ;
    float cFarZ;
    float cTotalTime;
    float cDeltaTime;

    Light cLights[NUM_MAX_LIGHT];
    
    int renderType;
};

Texture2D tAlbedlMap : register(t0);
Texture2D tNormalMap : register(t1);
Texture2D tEmissiveMap : register(t2);
Texture2D tMetallicMap : register(t3);
Texture2D tRoughnessMap : register(t4);
Texture2D tAmbientMap : register(t5);
Texture2D tOpacityMap : register(t6);

SamplerState sAnisotropicWrap : register(s0);
SamplerState sAnisotropicClamp : register(s1);
SamplerState sLinearWrap : register(s2);
SamplerState sLinearClamp : register(s3);
SamplerState sLinearPointWrap : register(s4);
SamplerState sLinearPointClamp : register(s5);

struct PSIN
{
    float4 PosH : SV_POSITION;
    float4 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 TexCoord : TEXCOORD;
    float3 Tangent : TANGENT;
    float3 BiTangent : BITANGENT;
};

PBRIN SamplerPBR(PSIN psin)
{
    PBRIN pbin;

    if (material.cB0 == 1)
    {
        pbin.albedo = pow(material.cBaseColor_0.rgb, 2.2);
    }
    else
    {
        pbin.albedo = pow(tAlbedlMap.Sample(sAnisotropicWrap, psin.TexCoord).rgb, 2.2);
    }

    if (material.cB1 == 1)
    {
        pbin.normal = psin.NormalW;
    }
    else
    {
        pbin.normal = tNormalMap.Sample(sAnisotropicWrap, psin.TexCoord).rgb;
    }
    
    if (material.cB2 == 1)
    {
        pbin.emissive = material.cEmissiveColor_2, 2.2;
    }
    else
    {
        pbin.emissive = tEmissiveMap.Sample(sAnisotropicWrap, psin.TexCoord).rgb;
    }
    pbin.emissive = pbin.emissive * material.cEmissiveIntensity_3;
    
    if (material.cB4 == 1)
    {
        pbin.metallic = material.cMetallic_4;
    }
    else
    {
        pbin.metallic = tMetallicMap.Sample(sAnisotropicWrap, psin.TexCoord).b;
    }
    
    if (material.cB5 == 1)
    {
        pbin.roughness = material.cRoughness_5;
    }
    else
    {
        pbin.roughness = tRoughnessMap.Sample(sAnisotropicWrap, psin.TexCoord).g;
    }

    if (material.cB6 == 1)
    {
        pbin.ao = material.cAo_6;
    }
    else
    {
        pbin.ao = tAmbientMap.Sample(sAnisotropicWrap, psin.TexCoord).r;
    }
    return pbin;
}

float GetOpacity(PSIN psin)
{
    if (material.cB7 == 1)
    {
        if (material.cB0 == 0)
        {
            return tAlbedlMap.Sample(sAnisotropicWrap, psin.TexCoord).a;
        }
        else
        {
            return material.cOpacity_7;
        }
    }
    else
        return tOpacityMap.Sample(sAnisotropicWrap, psin.TexCoord).a;
}

float4 PS(PSIN psin) : SV_Target
{
    float3 T = normalize(psin.Tangent);
    float3 B = normalize(psin.BiTangent);
    float3 N = normalize(psin.NormalW);
    
    float3x3 TBN = float3x3(T, B, N);
    
    PBRIN pbin = SamplerPBR(psin);
    float alpha = GetOpacity(psin);
    
    pbin.normal = normalize(pbin.normal * 2.0f - 1.0f);
    pbin.normal = mul(pbin.normal, TBN);
    
    pbin.cameraPos = cCameraPosW;
    pbin.fragPos = psin.PosW.xyz;
    pbin.NumLight = cLightNum;
    pbin.lights = cLights;

    float3 pbrColor = PBR(pbin, renderType);
    float4 color = float4(pbrColor, alpha);
    return color;
}