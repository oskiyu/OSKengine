#version 460

#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nouniform_qualifier : enable

struct RayResult {
    vec4 color;
    float distance;
    vec3 normal;
};

layout (location = 0) rayPayloadInEXT RayResult rayResult;

hitAttributeEXT vec2 attribs;

layout (binding = 0, set = 0) uniform accelerationStructureEXT topLevelAccelerationStructure;

layout(binding = 3, set = 0) buffer Vertices { vec4 v[]; } vertices;
layout(binding = 4, set = 0) buffer Indices { uint i[]; } indices;

struct Vertex {
    vec3 position;
    vec3 normal;
    vec4 color;
    vec2 texCoords;
};

Vertex GetVertex(uint index) {
    Vertex v;

    const int m = (4 * 12) / (4 * 4);

    vec4 data0 = vertices.v[m * index + 0];
	vec4 data1 = vertices.v[m * index + 1];
	vec4 data2 = vertices.v[m * index + 2];
	vec4 data3 = vertices.v[m * index + 3];

    v.position = data0.xyz;
    v.normal = vec3(data0.w, data1.x, data1.y);
    v.color = vec4(data1.z, data2.x, data2.y, data2.z);
    v.texCoords = data3.xy;

    return v;
}

void main() {
    const ivec3 index = ivec3(
        indices.i[3 * gl_PrimitiveID + 0],
        indices.i[3 * gl_PrimitiveID + 1],
        indices.i[3 * gl_PrimitiveID + 2]
    );

    Vertex v0 = GetVertex(index.x);
    Vertex v1 = GetVertex(index.y);
    Vertex v2 = GetVertex(index.z);

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

    rayResult.color = vec4(1.0);// color;
    rayResult.distance = gl_RayTmaxEXT;
    rayResult.normal = normal;
}
