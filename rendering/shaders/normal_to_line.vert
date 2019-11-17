#version 430

layout(location=0)in vec4 Position;
layout(location=1)in vec3 Normal;

out vec3 normal;

void main() {
    normal = Normal;
    gl_Position = Position;
}