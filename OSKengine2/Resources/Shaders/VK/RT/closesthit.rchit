#version 460

#extension GL_EXT_ray_tracing : require

struct RayResult {
    vec4 color;
    float distance;
    vec3 normal;
};

layout (location = 0) rayPayloadInEXT RayResult rayResult;

hitAttributeEXT vec2 attribs;

layout (binding = 0, set = 0) uniform accelerationStructureEXT topLevelAccelerationStructure;

struct InstanceInfo {
    uint vertexOffset;
    uint indexOffset;
};

layout(binding = 3, set = 0) buffer Vertices { vec4 data[]; } vertices;
layout(binding = 4, set = 0) buffer Indices { uint data[]; } indices;
layout(binding = 5, set = 0) buffer InstanceInfos { InstanceInfo data[]; } instanceInfos;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec4 color;
    vec2 texCoords;
};

Vertex GetVertex(uint index, InstanceInfo info) {
    Vertex v;

    // size: 4 * 12 bytes                   vec4 size = 4 * 4 bytes
    const uint vec4OffsetPerVertex = (4 * 12) / (4 * 4);
    const uint initialVec4Offset = info.vertexOffset / (4 * 4);

    vec4 data0 = vertices.data[initialVec4Offset + vec4OffsetPerVertex * index + 0];
	vec4 data1 = vertices.data[initialVec4Offset + vec4OffsetPerVertex * index + 1];
	vec4 data2 = vertices.data[initialVec4Offset + vec4OffsetPerVertex * index + 2];

    v.position = data0.xyz;
    v.normal = vec3(data0.w, data1.x, data1.y);
    v.color = vec4(data1.z, data1.w, data2.x, data2.y);
    v.texCoords = data2.zw;

    return v;
}

void main() {
    const InstanceInfo instanceInfo = instanceInfos.data[gl_InstanceID];

    const uint indexOffset = instanceInfo.indexOffset / (4);
    const ivec3 index = ivec3(
        indices.data[indexOffset + 3 * gl_PrimitiveID + 0],
        indices.data[indexOffset + 3 * gl_PrimitiveID + 1],
        indices.data[indexOffset + 3 * gl_PrimitiveID + 2]
    );

    Vertex v0 = GetVertex(index.x, instanceInfo);
    Vertex v1 = GetVertex(index.y, instanceInfo);
    Vertex v2 = GetVertex(index.z, instanceInfo);

    const vec3 barCoords = vec3(
        1.0f - attribs.x - attribs.y,
        attribs.x,
        attribs.y
    );

    vec3 normal = normalize(
        v0.normal * barCoords.x + 
        v1.normal * barCoords.y + 
        v2.normal * barCoords.z
    );

    vec4 color = (
        v0.color * barCoords.x +
        v1.color * barCoords.y +
        v2.color * barCoords.z
    );

    rayResult.color = vec4(0.2, 0.8, 0.2, 1.0);
    if (gl_InstanceCustomIndexEXT == 0) {
        rayResult.color = vec4(0.2, 0.2, 0.2, 1.0);
    }

    rayResult.distance = gl_RayTmaxEXT;
    rayResult.normal = normal;
}
