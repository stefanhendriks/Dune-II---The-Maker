struct VertexInput {
    float2 pos   : POSITION; // Sera automatiquement mappé à la Location 0
};

struct VertexOutput {
    float4 position : SV_Position;
};

VertexOutput VS_Main(VertexInput input) {
    VertexOutput output;
    output.position = float4(input.pos, 0.0f, 1.0f);
    return output;
}