#version 430
layout(location=0) in vec4 Position;
layout(location=1)uniform mat4 mvpUniform;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main() {
    gl_Position = mvpUniform * Position;
}