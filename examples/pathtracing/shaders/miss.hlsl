struct Payload
{
    [[vk::location(0)]] float3 intersection;
};

[shader("miss")]
void main(inout Payload p)
{
    p.intersection = float3(1.0, 0.0, 0.0);
}
