struct PSIN
{
    float4 PosH : SV_POSITION;
    float4 PosW : POSITION;
};

cbuffer worldMatr2 : register(b2)
{
    float4 xColor;
    float4 zColor;
    float4 cColor;
};

float4 PS(PSIN psin) : SV_Target
{
    if (abs(psin.PosW.x - 0.0f) < 0.1f)
    {
        return zColor;
    }
    else if (abs(psin.PosW.z - 0.0f) < 0.1f)
    {
        return xColor;
    }
    return cColor;
}