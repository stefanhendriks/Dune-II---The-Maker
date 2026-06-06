struct VertexInput {
    float2 pos   : POSITION;
    float4 color : COLOR0;
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
