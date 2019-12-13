#version 430

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 normal;

uniform mat4 lightViewProjectionMatrix;
uniform mat4 model;

void main() {
    gl_Position = lightViewProjectionMatrix * model * aPos;
}