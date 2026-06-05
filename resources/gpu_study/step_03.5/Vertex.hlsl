struct VertexInput {
    float2 pos   : POSITION; // Sera automatiquement mappé à la Location 0
    float4 color : COLOR0;   // Sera automatiquement mappé à la Location 1
};

struct VertexOutput {
    float4 position : SV_Position;
    float4 color    : COLOR0;
};

VertexOutput VS_Main(VertexInput input) {
    VertexOutput output;
    output.position = float4(input.pos, 0.0f, 1.0f);
    output.color = input.color;
    return output;
}