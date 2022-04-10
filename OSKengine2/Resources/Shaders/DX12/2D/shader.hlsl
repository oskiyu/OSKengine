struct VS2PS {
    float2 texCoords : TEXCOORD;
    float4 color : COLOR;
};

struct PushConstants {
    float4x4 modelMatrix;
    float4 color;
    float4 texCoords;
};
ConstantBuffer<PushConstants> pushConstants : register(b0);

cbuffer CameraBuffer : register(b1) {
    float4x4 cameraView;
};

Texture2D texture0 : register(t0);
SamplerState textureSampler : register(s0);

VS2PS vmain(float2 position : POSITION, float2 texCoords : TEXCOORD) {
    VS2PS result;

    float4x4 temp = mul(cameraProjection, cameraView);
    temp = mul(temp, pushConstants.modelMatrix);

    result.position = mul(temp, float4(position, 1.0));
    result.color = pushConstants.color;
    result.texCoords = texCoords;

    return result;
}

float4 fmain(VS2PS input) : SV_Target {
    return input.color * texture0.Sample(textureSampler, input.texCoords);
}
