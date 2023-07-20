struct Payload
{
    [[vk::location(0)]] float3 intersection;
};

[shader("closesthit")]
void main(inout Payload p, in float2 attribs)
{
    const float3 barycentricCoords = float3(1.0f - attribs.x - attribs.y, attribs.x, attribs.y);
    p.intersection = float3(0, 1, 0);
    //barycentricCoords;
}
