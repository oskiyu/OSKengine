struct VS2PS {
    float4 position : SV_Position;
    float4 color : COLOR;
    float2 texCoords : TEXCOORD;
};

cbuffer UniformBuffer : register(b0) {
    float4x4 model;
};

VS2PS vmain(float3 position : POSITION, float4 color : COLOR, float2 texCoords : TEXCOORD) {
    VS2PS result;

    result.position = mul(model, float4(position, 1.0));
    result.color = color;

    return result;
}

float4 fmain(VS2PS input) : SV_Target {
    return input.color;
}
