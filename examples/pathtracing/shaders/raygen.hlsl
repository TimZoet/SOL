[[vk::binding(0)]]
RaytracingAccelerationStructure rs;

[[vk::binding(1)]]
RWTexture2D<float4> image;

struct Payload
{
    [[vk::location(0)]] float3 intersection;
};

[shader("raygeneration")]
void main()
{
    uint3 LaunchID = DispatchRaysIndex();
    uint3 LaunchSize = DispatchRaysDimensions();

    const float2 origin = (float2(LaunchID.xy) + float2(0.5, 0.5)) / float2(LaunchSize.xy) * 2.0 - 1.0;

    RayDesc rayDesc;
    rayDesc.Origin = float3(origin, 1);
    rayDesc.Direction = float3(0, 0, -1);
    rayDesc.TMin = 0.001;
    rayDesc.TMax = 10000.0;

    Payload payload;
    TraceRay(
        rs,
        RAY_FLAG_FORCE_OPAQUE,
        0xff,
        0, // R_offset RayContributionToHitGroupIndex
        0, // R_stride MultiplierForGeometryContributionToHitGroupIndex
        0, // R_miss   MissShaderIndex
        rayDesc,
        payload
    );

    image[int2(LaunchID.xy)] = float4(payload.intersection, 1.0);
}
