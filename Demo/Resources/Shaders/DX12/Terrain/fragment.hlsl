struct Input {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 texCoords : TEXCOORD;
};

Texture2D texture0 : register(t1);
SamplerState textureSampler : register(s1);

float4 main(Input input) : SV_Target {
    return input.color * texture0.Sample(textureSampler, input.texCoords);
}
