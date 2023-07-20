[[vk::binding(0)]]
RWTexture2D<float4> g_texture;

[numthreads(16, 16, 1)]
void CSMain(
    uint3 groupId : SV_GroupID,
    uint3 groupThreadId : SV_GroupThreadID,
    uint3 dispatchThreadId : SV_DispatchThreadID,
    uint groupIndex : SV_GroupIndex
)
{
    float w, h;
    g_texture.GetDimensions(w, h);

    if (dispatchThreadId.x >= w || dispatchThreadId.y >= h)
        return;

    g_texture[dispatchThreadId.xy] = float4(dispatchThreadId.xy / float2(w, h), 0.0, 1.0);
}
