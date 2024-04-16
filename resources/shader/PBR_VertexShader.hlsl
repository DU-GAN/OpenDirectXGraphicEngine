#include "Vertex_Auxiliary.hlsl"
#include "PBR_Auxiliary.hlsl"

cbuffer worldMatr : register(b0)
{
    float4x4 cWorld;
    float4x4 cInvTransWorld;
};

cbuffer commonData : register(b2)
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

VSOUT VS(VSIN_G vin)
{
    VSOUT vout = (VSOUT) 0.0f;

    vout.PosW = mul(float4(vin.PosL, 1.0f), cWorld);
    vout.PosH = mul(vout.PosW, cViewProj);
    vout.NormalW = mul(vin.Normal, (float3x3) cInvTransWorld);
    vout.Tangent = mul(vin.Tangent, (float3x3) cInvTransWorld);
    vout.BiTangent = mul(vin.BiTangent, (float3x3) cInvTransWorld);
    vout.TexCoord = vin.TexCoord;
    
    return vout;
}