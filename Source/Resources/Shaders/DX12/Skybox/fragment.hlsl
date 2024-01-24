struct Input {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float3 texCoords : TEXCOORD;
};

TextureCube texture0 : register(t0);
SamplerState textureSampler : register(s0);

float4 main(Input input) : SV_Target {
    return input.color * texture0.Sample(textureSampler, input.texCoords);
}
