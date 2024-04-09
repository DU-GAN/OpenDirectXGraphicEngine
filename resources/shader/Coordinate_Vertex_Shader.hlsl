#include "PBR_Auxiliary.hlsl"

struct VSIN
{
    float3 PosL : POSITION;
};

struct VSOUT
{
    float4 PosH : SV_POSITION;
    float4 PosW : POSITION;
};

cbuffer worldMatr1 : register(b0)
{
    float4x4 cWorld;
    float4x4 cInvTransWorld;
};

cbuffer commonData : register(b1)
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
};

VSOUT VS(VSIN vin)
{
    VSOUT vout;

    vout.PosW = mul(float4(vin.PosL, 1.0f), cWorld);
    vout.PosH = mul(vout.PosW, cViewProj);
    
    return vout;
}