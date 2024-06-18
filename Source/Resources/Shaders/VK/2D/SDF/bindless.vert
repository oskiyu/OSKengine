#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 inPos;

layout(location = 0) flat out int outShapeType;
layout(location = 1) flat out int outContentType;
layout(location = 2) out vec4 outMainColor;
layout(location = 3) out vec4 outSecondaryColor;
layout(location = 4) out vec2 outTexCoords;
layout(location = 5) flat out vec2 outCenterPosition;
layout(location = 6) flat out float outBorderWidth;
layout(location = 7) flat out int outInstanceIndex;
layout(location = 8) flat out int outFill;

// Material.
layout (set = 0, binding = 0) uniform GlobalInformation {
    vec2 resolution;
    mat4 cameraMatrix;
} globalInformation;

layout (set = 1, binding = 0) uniform DrawCall {
    ivec3 typeInfos;
    vec4 texCoords;
    vec4 mainColor;
    vec4 secondaryColor;
    vec4 floatInfos;
    mat4 matrix;
} drawCalls[];

void main() {
    outInstanceIndex = gl_InstanceIndex;
    
    outShapeType = drawCalls[gl_InstanceIndex].typeInfos.x;
    outContentType = drawCalls[gl_InstanceIndex].typeInfos.y;
    outFill = drawCalls[gl_InstanceIndex].typeInfos.z;

    outBorderWidth = drawCalls[gl_InstanceIndex].floatInfos.z;

    outMainColor = drawCalls[gl_InstanceIndex].mainColor;
    outSecondaryColor = drawCalls[gl_InstanceIndex].mainColor;

    outTexCoords = drawCalls[gl_InstanceIndex].texCoords.xy + inPos.xy * drawCalls[gl_InstanceIndex].texCoords.zw;

    outCenterPosition = drawCalls[gl_InstanceIndex].floatInfos.xy;
    
    gl_Position = drawCalls[gl_InstanceIndex].matrix * vec4(inPos, 0.0, 1.0);
}
