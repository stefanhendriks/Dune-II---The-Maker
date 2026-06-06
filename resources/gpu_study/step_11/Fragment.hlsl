struct VertexToPixel {
    float4 position : SV_Position;
    float4 color    : COLOR0;
};

float4 PS_Main(VertexToPixel input) : SV_Target {
    return input.color;
}