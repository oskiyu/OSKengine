dxc -T vs_6_1 -E main -Fo vertex.shader .\vertex.hlsl
dxc -T ps_6_1 -E main -Fo fragment.shader .\fragment.hlsl
dxc -T hs_6_1 -E main -Fo control.shader .\control.hlsl
dxc -T ds_6_1 -E main -Fo evaluation.shader .\evaluation.hlsl
PAUSE