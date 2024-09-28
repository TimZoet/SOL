struct VSInput
{
    float3 position : POSITION;
};

struct VSOutput
{
    float4 position : SV_POSITION;
};

[[vk::binding(0, 0)]]
cbuffer CameraMatrix
{
    float4x4 wvp;
}

[[vk::binding(1, 0)]]
cbuffer ModelMatrix
{
    float4x4 model;
}

VSOutput main(VSInput input)
{
    VSOutput result;

    result.position = float4(input.position, 1.0f);//mul(float4(input.position, 1.0f), model);

    return result;
}

