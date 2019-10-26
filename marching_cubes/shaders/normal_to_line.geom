#version 430
layout(points) in;
layout(line_strip, max_vertices = 8) out;
uniform mat4 mvpUniform;

in vec3 normal[];

out gl_PerVertex
{
    vec4 gl_Position;
};

void main() {
    //return;
    vec4 p2 = vec4(gl_in[0].gl_Position.xyz + 0.05 * normal[0], 1);
    gl_Position = mvpUniform * gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = mvpUniform * p2;
    EmitVertex();
    EndPrimitive();

    float normTargetSize = 0.01;

    gl_Position = mvpUniform * (p2 + vec4(normTargetSize, 0, 0, 0));
    EmitVertex();
    gl_Position = mvpUniform * (p2 - vec4(normTargetSize, 0, 0, 0));
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * (p2 + vec4(0, normTargetSize, 0, 0));
    EmitVertex();
    gl_Position = mvpUniform * (p2 - vec4(0, normTargetSize, 0, 0));
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * (p2 + vec4(0, 0, normTargetSize, 0));
    EmitVertex();
    gl_Position = mvpUniform * (p2 - vec4(0, 0, normTargetSize, 0));
    EmitVertex();
    EndPrimitive();
}