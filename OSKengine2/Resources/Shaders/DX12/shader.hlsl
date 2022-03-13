struct VS2PS {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texCoords : TEXCOORD;
};

struct PushConstants {
    float4x4 modelMatrix;
};
ConstantBuffer<PushConstants> pushConstants : register(b0);

cbuffer CameraBuffer : register(b1) {
    float4x4 cameraView;
    float4x4 cameraProjection;
};

Texture2D texture0 : register(t0);
SamplerState textureSampler : register(s0);

VS2PS vmain(float3 position : POSITION, float4 color : COLOR, float2 texCoords : TEXCOORD) {
    VS2PS result;

    float4x4 temp = mul(cameraProjection, cameraView);
    temp = mul(temp, pushConstants.modelMatrix);

    result.position = mul(temp, float4(position, 1.0));
    result.color = color;
    result.texCoords = texCoords;

    return result;
}

float4 fmain(VS2PS input) : SV_Target {
    return input.color * texture0.Sample(textureSampler, input.texCoords);
}
