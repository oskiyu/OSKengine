dxc -T vs_6_1 -E vmain -Fo vertex.shader .\shader.hlsl
dxc -T ps_6_1 -E fmain -Fo fragment.shader .\shader.hlsl
PAUSE