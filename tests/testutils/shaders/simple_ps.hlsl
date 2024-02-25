struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

[[vk::binding(0, 1)]]
Texture2D<float4> color;
[[vk::binding(1, 1)]]
SamplerState s;

float4 main(PSInput input) : SV_TARGET0
{
    return color.Sample(s, input.uv);
}
