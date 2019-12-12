#version 430

layout (location = 0) in vec4 Position;
layout (location = 1) in vec3 Normal;

const int NUM_CASCADES = 3;

uniform mat4 lightSpaceMatrix[NUM_CASCADES];
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 LightSpacePos[NUM_CASCADES];
out float clipSpacePos;
out vec3 Normal0;
out vec3 WorldPos0;

void main()
{
    gl_Position = projection * view * model * Position;

    for (int i = 0 ; i < NUM_CASCADES ; i++) {
        LightSpacePos[i] = lightSpaceMatrix[i] * Position;
    }

    clipSpacePos = gl_Position.z;
    Normal0 = transpose(inverse(mat3(model))) * Normal;
    WorldPos0 = (model * Position).xyz;
}