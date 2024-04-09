#include "Vertex_Auxiliary.hlsl"
#include "PBR_Auxiliary.hlsl"

cbuffer worldMatr : register(b0)
{
    float4x4 cWorld;
    float4x4 cInvTransWorld;
};

cbuffer skinned : register(b2)
{
    float4x4 cBoneTransforms[96];
}

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

VSOUT VS(VSIN_GB vin)
{
    VSOUT vout = (VSOUT) 0.0f;

    float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    weights[0] = vin.BoneWeights.x;
    weights[1] = vin.BoneWeights.y;
    weights[2] = vin.BoneWeights.z;
    weights[3] = 1.0f - weights[0] - weights[1] - weights[2];
    
    float3 posL = float3(0.0f, 0.0f, 0.0f);
    float3 normalL = float3(0.0f, 0.0f, 0.0f);
    float3 tangentL = float3(0.0f, 0.0f, 0.0f);
    float3 bitangentL = float3(0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < 4; ++i)
    {
        if (vin.BoneIds[i] == -1)
            continue;
        posL += weights[i] * mul(float4(vin.PosL, 1.0f), cBoneTransforms[vin.BoneIds[i]]).xyz;
        normalL += weights[i] * mul(vin.Normal, (float3x3) cBoneTransforms[vin.BoneIds[i]]);
        tangentL += weights[i] * mul(vin.Tangent.xyz, (float3x3) cBoneTransforms[vin.BoneIds[i]]);
        bitangentL += weights[i] * mul(vin.BiTangent.xyz, (float3x3) cBoneTransforms[vin.BoneIds[i]]);
    }
    
    vin.PosL = posL;
    vin.Normal = normalL;
    vin.Tangent.xyz = tangentL;
    vin.BiTangent.xyz = bitangentL;
    
    vout.PosW = mul(float4(vin.PosL, 1.0f), cWorld);
    vout.PosH = mul(vout.PosW, cViewProj);
    vout.NormalW = mul(vin.Normal, (float3x3) cInvTransWorld);
    vout.Tangent = mul(vin.Tangent, (float3x3) cInvTransWorld);
    vout.BiTangent = mul(vin.BiTangent, (float3x3) cInvTransWorld);
    vout.TexCoord = vin.TexCoord;

    return vout;
}