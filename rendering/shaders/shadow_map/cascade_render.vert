#version 430

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec3 aNormal;

const int NUM_CASCADES = 3;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 Position;

void main()
{
    gl_Position = projection * view * model * aPosition;

    Normal = transpose(inverse(mat3(model))) * aNormal;
    Position = (model * aPosition).xyz;
}