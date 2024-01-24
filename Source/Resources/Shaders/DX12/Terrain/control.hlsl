// Output patch.
struct ControlPatchOutput {
    float outerFactor[3]         : SV_TessFactor;
    float innerFactor           : SV_InsideTessFactor;
};

struct Output {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 texCoords : TEXCOORD;
};

struct Input {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 texCoords : TEXCOORD;
};

struct PushConstants {
    float4x4 modelMatrix;
};
ConstantBuffer<PushConstants> pushConstants : register(b0, space1);

cbuffer CameraBuffer : register(b1) {
    float4x4 cameraView;
    float4x4 cameraProjection;

    float3 cameraPos;
};

float4 GetPos(float4 pre) {
    float4x4 temp = mul(cameraProjection, cameraView);
    temp = mul(temp, pushConstants.modelMatrix);

    return mul(temp, pre);
}

ControlPatchOutput ControlMain(InputPatch<Input, 3> input, uint PatchID : SV_PrimitiveID) {
    ControlPatchOutput output;
    
    const int minTesselation = 1;
    const int maxTesselation = 32;

    const float minDistance = 0.2;
    const float maxDistance = 9.2;

    float4 pos0 = GetPos(input[0].position); 
    float4 pos1 = GetPos(input[1].position); 
    float4 pos2 = GetPos(input[2].position); 

    float4 middlePos0 = lerp(pos2, pos1, 0.5);
    float4 middlePos1 = lerp(pos0, pos2, 0.5);
    float4 middlePos2 = lerp(pos1, pos0, 0.5);

    float clampedDistance0 = clamp((abs(middlePos0.z) - minDistance) / maxDistance, 0.0, 1.0);
    float clampedDistance1 = clamp((abs(middlePos1.z) - minDistance) / maxDistance, 0.0, 1.0);
    float clampedDistance2 = clamp((abs(middlePos2.z) - minDistance) / maxDistance, 0.0, 1.0);

    // Nivel: bordes del triángulo.
    float level0 = lerp(maxTesselation, minTesselation, clampedDistance0);
    float level1 = lerp(maxTesselation, minTesselation, clampedDistance1);
    float level2 = lerp(maxTesselation, minTesselation, clampedDistance2);

    float4 triMidPosition = (pos0 + pos1 + pos2) / 3;

    output.outerFactor[0] = level0;
    output.outerFactor[1] = level1;
    output.outerFactor[2] = level2;
    output.innerFactor = lerp(level0, lerp(level1, level2, 0.5), 0.5);

    return output;
}

[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ControlMain")]
Output main(InputPatch<Output, 3> input, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID) {
    Output output;

    output.position = input[i].position;
    output.normal = input[i].normal;
    output.color = input[i].color;
    output.texCoords = input[i].texCoords;

    return output;
}
