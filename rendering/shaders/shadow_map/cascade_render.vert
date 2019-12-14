#version 430

layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 v2fNormal;
out vec3 v2fPosition;
out vec3 FragPos;


void main()
{
    mat4 modelView = view * model;

    v2fPosition = (modelView * aPosition).xyz;
    v2fNormal = normalize(transpose(inverse(mat3(model))) * aNormal);
    FragPos = (model * aPosition).xyz;
    gl_Position = projection * modelView * aPosition;
}