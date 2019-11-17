#version 430

layout(points) in;
layout(line_strip, max_vertices = 6) out;

uniform mat4 mvpUniform;
uniform vec3 start;
uniform float step;

out gl_PerVertex
{
    vec4 gl_Position;
};

vec3 offsetForEdge(uint edge) {
    switch (edge) {
        case 0:
            return vec3(0, 1, 0);
        case 3:
            return vec3(1, 0, 0);
        case 8:
            return vec3(0, 0, 1);
    }
    return vec3(0, 0, 0);
}

void main() {
    uint dim = 32;

    uint z = uint(gl_PrimitiveIDIn / (dim * dim) % dim);
    uint y = uint(gl_PrimitiveIDIn / dim % dim);
    uint x = uint(gl_PrimitiveIDIn % dim);
    vec3 chunkCoord = vec3(x, y, z);

    vec3 coord = start + chunkCoord * step;

    gl_Position = mvpUniform * vec4(coord, 1);
    EmitVertex();
    gl_Position = mvpUniform * vec4(coord + offsetForEdge(0) * step, 1);
    EmitVertex();
    EndPrimitive();
    gl_Position = mvpUniform * vec4(coord, 1);
    EmitVertex();
    gl_Position = mvpUniform * vec4(coord + offsetForEdge(3) * step, 1);
    EmitVertex();
    EndPrimitive();
    gl_Position = mvpUniform * vec4(coord, 1);
    EmitVertex();
    gl_Position = mvpUniform * vec4(coord + offsetForEdge(8) * step, 1);
    EmitVertex();
    EndPrimitive();
}
