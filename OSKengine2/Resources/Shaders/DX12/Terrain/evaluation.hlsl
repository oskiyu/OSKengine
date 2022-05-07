struct Output {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 texCoords : TEXCOORD;
};

struct ControlPatchOutput {
    float outerFactor[3]         : SV_TessFactor;
    float innerFactor           : SV_InsideTessFactor;
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

Texture2D texture0 : register(t0);
SamplerState textureSampler : register(s0);

[domain("tri")]
Output main(ControlPatchOutput controlOutput, float3 domain : SV_DomainLocation, const OutputPatch<Output, 3> patch) {
    Output output;

    output.normal = domain.x * patch[0].normal + domain.y * patch[1].normal + domain.z * patch[2].normal;
    output.texCoords = domain.x * patch[0].texCoords + domain.y * patch[1].texCoords + domain.z * patch[2].texCoords;
    output.color = patch[0].color;

    float4 pos = domain.x * patch[0].position + domain.y * patch[1].position + domain.z * patch[2].position;
    pos.y += texture0.SampleLevel(textureSampler, output.texCoords, 0).r;
    float4x4 temp = mul(cameraProjection, cameraView);
    temp = mul(temp, pushConstants.modelMatrix);

    output.position = mul(temp, pos);

    return output;
}
