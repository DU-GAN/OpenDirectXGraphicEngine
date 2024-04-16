struct VSIN_G
{
    float3 PosL : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float3 Tangent : TANGENT;
    float3 BiTangent : BITANGENT;
};

struct VSIN_GB
{
    float3 PosL : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
    float3 Tangent : TANGENT;
    float3 BiTangent : BITANGENT;
    uint4 BoneIds : BONEIDS;
    float3 BoneWeights : BONEWEIGHTS;
};

struct VSOUT
{
    float4 PosH : SV_POSITION;
    float4 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 TexCoord : TEXCOORD;
    float3 Tangent : TANGENT;
    float3 BiTangent : BITANGENT;
};

struct VSOUT_PH
{
    float4 PosH : SV_POSITION;
    float4 PosW : POSITION;
};