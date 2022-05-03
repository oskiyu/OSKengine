struct VS2PS {
    float4 position : SV_POSITION;
    float2 texCoords : TEXCOORD;
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

VS2PS vmain(float2 position : POSITION, float2 texCoords : BASETEXCOORDS) {
    VS2PS result;

    float4x4 temp = mul(cameraView, pushConstants.modelMatrix);
    result.position = mul(temp, float4(position.xy, 0.0, 1.0));
    result.texCoords = pushConstants.texCoords.xy + position * pushConstants.texCoords.zw;

    return result;
}

float4 fmain(VS2PS input) : SV_Target {

    return pushConstants.color * texture0.Sample(textureSampler, input.texCoords);
}
