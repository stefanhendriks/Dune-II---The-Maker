struct VertexToPixel {
    float4 position : SV_Position;
    float4 color    : COLOR0;
};

VertexToPixel VS_Main(uint vertexID : SV_VertexID) {
    VertexToPixel output;

    float2 positions[3] = {
        float2( 0.0f,  0.5f),
        float2( 0.5f, -0.5f),
        float2(-0.5f, -0.5f)
    };

    float4 colors[3] = {
        float4(1.0f, 0.0f, 0.0f, 1.0f),
        float4(0.0f, 1.0f, 0.0f, 1.0f),
        float4(0.0f, 0.0f, 1.0f, 1.0f)
    };

    output.position = float4(positions[vertexID], 0.0f, 1.0f);
    output.color = colors[vertexID];

    return output;
}
