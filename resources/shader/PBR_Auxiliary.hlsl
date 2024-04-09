#include "Light_Auxiliary.hlsl"

struct PBRIN
{
    float3 albedo;
    float metallic;

    float3 cameraPos;
    float roughness;

    float3 fragPos;
    float ao;

    float3 normal;
    int NumLight;

    Light lights[NUM_MAX_LIGHT];
    
    float3 emissive;
};

struct PBR_Material
{
    float4 cBaseColor_0;
    int cB0;
    
    float3 cNormal_1;
    int cB1;
    
    float3 cEmissiveColor_2;
    int cB2;
    
    float cEmissiveIntensity_3;
    int cB3;
    
    float cMetallic_4;
    int cB4;
    
    float cRoughness_5;
    int cB5;
  
    float cAo_6;
    int cB6;
    
    float cOpacity_7;
    int cB7;
};

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);

    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float3 PBR(PBRIN pbin,int type)
{
    float3 N = normalize(pbin.normal);
    float3 V = normalize(pbin.cameraPos - pbin.fragPos);

    float3 F0 = { 0.04f, 0.04f, 0.04f };
    F0 = lerp(F0, pbin.albedo, pbin.metallic);

    float3 Lo = { 0.0f, 0.0f, 0.0f };
    float3 xT = { 1.0f, 1.0f, 1.0f };

    float3 color;
    
    if(type == 0)
    {
        for (int i = 0; i < pbin.NumLight; ++i)
        {
            float3 L = normalize(pbin.lights[i].pos - pbin.fragPos);
            float3 H = normalize(V + L);

            float attenuation = 1.0f;

            if (pbin.lights[i].type == 1)
            {
                float dis = length(pbin.lights[i].pos - pbin.fragPos);
                float attenuation = 1.0 / (dis * dis);
            }
    
            float3 radiance = pbin.lights[i].radiant * attenuation;

            float NDF = DistributionGGX(N, H, pbin.roughness);
            float G = GeometrySmith(N, V, L, pbin.roughness);
            float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
            float3 kS = F;
            float3 kD = xT - kS;
            kD *= 1.0 - pbin.metallic;

            float3 nominator = NDF * G * F;
            float denominator = 4.0f *
            max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;

            float3 specular = nominator / denominator;
            float NdotL = max(dot(N, L), 0.0);
    
            Lo += (kD * pbin.albedo / PI + specular) * radiance * NdotL;
        }
        float3 ambient = pbin.emissive;
        color = ambient + Lo * pbin.ao;
    }else if(type==1)
    {
        if(pbin.ao >= 0.5)
            color = pbin.albedo * pbin.ao;
        else
            color = pbin.albedo;
    }else if(type==2)
    {
        color = float3(1.0f, 1.0f, 1.0f) * pbin.ao;
    }else if(type==3)
    {
        color = float3(.0f, .0f, .0f);
    }
    
    color = color / (color + xT);
    float3 xT2 = { 1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f };
    color = pow(color, xT2);

    return color;
}