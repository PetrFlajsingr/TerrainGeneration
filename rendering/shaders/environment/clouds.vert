#version 430

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Pos;
out vec3 Normal;

void main()
{
    mat4 modelView = view * model;
    gl_Position = projection * modelView * aPosition;
    Pos = (model * aPosition).xyz;
    Normal = normalize(transpose(inverse(mat3(model))) * aNormal);
}