struct VSInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD0;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

[[vk::binding(0, 0)]]
cbuffer cb
{
    float4x4 worldViewProj;
};

PSInput main(VSInput input)
{
    PSInput result;
    result.position = mul(float4(input.position, 1.0f), worldViewProj);
    result.uv = input.uv;
    return result;
}
