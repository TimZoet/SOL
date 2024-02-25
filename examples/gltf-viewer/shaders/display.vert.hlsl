struct VSInput
{
    float2 position : POSITION;
};

struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput main(VSInput input)
{
    VSOutput result;

    result.position = float4(input.position.xy, 0.0f, 1.0f);

    return result;
}
