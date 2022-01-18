struct VS2PS {
    float4 position : SV_Position;
    float4 color : COLOR;
};

VS2PS vmain(float3 position : POSITION, float4 color : COLOR) {
    VS2PS result;

    result.position = float4(position, 1.0);
    result.color = color;

    return result;
}

float4 fmain(VS2PS input) : SV_Target {
    return input.color;
}
