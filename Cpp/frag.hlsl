Texture2D<float4> stexture : register(t1, space0);
SamplerState _stexture_sampler : register(s1, space0);

static float4 outColor;
static float4 color;
static float2 texCoords;
static float3 normal;
static float3 position;

struct SPIRV_Cross_Input
{
    float3 position : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float4 color : TEXCOORD2;
    float2 texCoords : TEXCOORD3;
};

struct SPIRV_Cross_Output
{
    float4 outColor : SV_Target0;
};

void frag_main()
{
    outColor = ((color * float4(stexture.Sample(_stexture_sampler, texCoords).xyz, 1.0f)) * 0.89999997615814208984375f) + (float4(normal, 1.0f) * 0.100000001490116119384765625f);
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    color = stage_input.color;
    texCoords = stage_input.texCoords;
    normal = stage_input.normal;
    position = stage_input.position;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.outColor = outColor;
    return stage_output;
}

