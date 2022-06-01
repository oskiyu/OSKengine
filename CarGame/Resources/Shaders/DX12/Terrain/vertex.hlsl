struct Output {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
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

    float3 cameraPos;
};

Output main(float3 position : POSITION, float3 normal : NORMAL, float4 color : COLOR, float2 texCoords : TEXCOORD) {
    Output result;

    result.position = float4(position, 1.0);
    result.normal = normal;
    result.color = color;
    result.texCoords = texCoords;

    return result;
}
