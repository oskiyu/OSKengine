struct Output {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float3 texCoords : TEXCOORD;
};


// Camera UBO
cbuffer CameraBuffer : register(b0) {
    float4x4 cameraView;
    float4x4 cameraProjection;

    float3 cameraPos;
};

Output main(float3 position : POSITION, float3 normal : NORMAL, float4 color : COLOR, float2 texCoords : TEXCOORD) {
    Output result;
    
    float4x4 temp = mul(cameraProjection, cameraView);

    result.position = mul(temp, float4(position + cameraPos, 1.0));
    result.color = color;
    result.texCoords = position;

    return result;
}
