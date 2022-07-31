#version 460
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 inPosition;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform samplerCube image;

const float PI = 3.14159265359;

void main() {
    const vec3 pos = vec3(inPosition.x, -inPosition.y, inPosition.z); // ? -inPosition.y ?
    const vec3 normal = normalize(pos);
    const vec3 right = normalize(cross(vec3(0.0, 1.0, 0.0), normal));
    const vec3 up = normalize(cross(right, normal));

    vec3 accumulatedIrradiance = vec3(0.0);

    // Accuracy (menor = mejor).
    const float sampleDelta = 0.075;
    float numSamples = 0.0;

    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {

            const vec3 tangent = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            const vec3 sampleVec = tangent.x * right + tangent.y * up + tangent.z * normal; // N?

            accumulatedIrradiance += texture(image, sampleVec).rgb * cos(theta) * sin(theta);
            numSamples++;
        }
    }

    accumulatedIrradiance = PI * accumulatedIrradiance * (1.0 / float(numSamples));

    outColor = vec4(accumulatedIrradiance, 1.0);
}
