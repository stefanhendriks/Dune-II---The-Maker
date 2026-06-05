Texture2D LogoTexture : register(t0, space2);
SamplerState LogoSampler : register(s0, space2);

struct PixelInput {
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
};

float4 PS_Main(PixelInput input) : SV_Target0
{
    return LogoTexture.Sample(LogoSampler, input.uv);
}