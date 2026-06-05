struct PixelInput {
    float4 position : SV_Position;
    float4 color : COLOR0;
};

StructuredBuffer<float4> FragmentColors : register(t0, space2);

float4 PS_Main(PixelInput input) : SV_Target0
{
    return input.color * FragmentColors[0];
}