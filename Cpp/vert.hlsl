cbuffer Camera : register(b0, space0)
{
    row_major float4x4 camera_view : packoffset(c0);
    row_major float4x4 camera_projection : packoffset(c4);
};

cbuffer Model
{
    row_major float4x4 model_modelMatrix : packoffset(c0);
};


static float4 gl_Position;
static float3 outPosition;
static float3 inPosition;
static float4 outColor;
static float4 inColor;
static float3 outNormal;
static float3 inNormal;
static float2 outTexCoords;
static float2 inTexCoords;

struct SPIRV_Cross_Input
{
    float3 inPosition : POSITION;
    float3 inNormal : NORMAL;
    float4 inColor : COLOR;
    float2 inTexCoords : TEXCOORD;
};

struct SPIRV_Cross_Output
{
    float3 outPosition : TEXCOORD0;
    float3 outNormal : TEXCOORD1;
    float4 outColor : TEXCOORD2;
    float2 outTexCoords : TEXCOORD3;
    float4 gl_Position : SV_Position;
};

void vert_main()
{
    outPosition = inPosition;
    outColor = inColor;
    outNormal = inNormal;
    outTexCoords = inTexCoords;
    gl_Position = mul(float4(inPosition, 1.0f), mul(model_modelMatrix, mul(camera_view, camera_projection)));
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    inPosition = stage_input.inPosition;
    inColor = stage_input.inColor;
    inNormal = stage_input.inNormal;
    inTexCoords = stage_input.inTexCoords;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output.outPosition = outPosition;
    stage_output.outColor = outColor;
    stage_output.outNormal = outNormal;
    stage_output.outTexCoords = outTexCoords;
    return stage_output;
}

