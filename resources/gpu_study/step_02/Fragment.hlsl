struct PixelInput {
    float4 position : SV_Position;
    float4 color : COLOR0;
};

float4 PS_Main(PixelInput input) : SV_Target0
{
    return input.color;
}