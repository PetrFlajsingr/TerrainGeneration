#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;

out vec2 v2fTexcoords;

uniform mat4 modelMatrix;

void main()
{
    gl_Position = modelMatrix * vec4(position.xy, 0.0f, 1.0f);
    v2fTexcoords = texcoord;
}