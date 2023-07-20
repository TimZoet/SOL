struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

[[vk::combinedImageSampler]][[vk::binding(0)]]
Texture2D g_texture;
[[vk::combinedImageSampler]][[vk::binding(0)]]
SamplerState g_sampler;

float4 main(PSInput input) : SV_TARGET
{
    return g_texture.Sample(g_sampler, input.uv) * float4(input.position.xyz * 0.0005f, 1.0f);
}
