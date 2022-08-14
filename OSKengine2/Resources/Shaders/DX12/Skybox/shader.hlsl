struct VS2PS {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float3 texCoords : TEXCOORD;
};

cbuffer CameraBuffer : register(b0) {
    float4x4 cameraView;
    float4x4 cameraProjection;

    float3 cameraPos;
};

TextureCube texture0 : register(t0);
SamplerState textureSampler : register(s0);

VS2PS vmain(float3 position : POSITION, float3 normal : NORMAL, float4 color : COLOR, float2 texCoords : TEXCOORD) {
    VS2PS result;

    float4x4 temp = mul(cameraProjection, cameraView);

    result.position = mul(temp, float4(position + cameraPos, 1.0));
    result.color = color;
    result.texCoords = position;

    return result;
}

float4 fmain(VS2PS input) : SV_Target {
    return input.color * texture0.Sample(textureSampler, input.texCoords);
}
