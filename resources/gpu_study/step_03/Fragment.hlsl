StructuredBuffer<float4> FragmentColors : register(t0, space2);

float4 PS_Main() : SV_Target0
{
    return FragmentColors[0];
}