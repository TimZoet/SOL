struct VSInput
{
    float2 position : POSITION;
    float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOutput main(VSInput input)
{
    VSOutput result;

    result.position = float4(input.position.xy, 0.0f, 1.0f);
    result.uv = input.uv;

    return result;
}
