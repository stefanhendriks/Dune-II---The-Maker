struct PixelInput {
    float4 position : SV_Position;
    float4 color : COLOR0;
};

cbuffer FragmentUniforms : register(b0, space3)
{
    float uRedWave;
    float3 _padding;
};

float4 PS_Main(PixelInput input) : SV_Target0
{
    float redFactor = 0.5f + 0.5f * sin(uRedWave);
    float4 outColor = input.color;
    outColor.r = redFactor;
    return outColor;
}