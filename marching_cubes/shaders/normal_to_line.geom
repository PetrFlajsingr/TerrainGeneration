#version 430
layout(points) in;
layout(line_strip, max_vertices = 4) out;
uniform mat4 mvpUniform;

in vec3 normal[];

out gl_PerVertex
{
    vec4 gl_Position;
};

void main() {
    // if (gl_PrimitiveIDIn % 8 == 7 || gl_PrimitiveIDIn % 64 > 55 || gl_PrimitiveIDIn >= 448) return;
    vec4 p2 = vec4(gl_in[0].gl_Position.xyz + 0.1 * normal[0], 1);
    gl_Position = mvpUniform * gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = mvpUniform * p2;
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * (p2 + vec4(0.02, 0, 0, 0));
    EmitVertex();
    gl_Position = mvpUniform * (p2 - vec4(0.02, 0, 0, 0));
    EmitVertex();
    EndPrimitive();
}