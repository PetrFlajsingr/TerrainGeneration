#version 430

layout(points) in;
layout(points, max_vertices = 3) out;

in uint edgeFlags[];

out uint edgeMarker;

void main() {
    uint xyz = edgeFlags[0] & 0xFFFFFF00u;

    if ((edgeFlags[0] & 1u) != 0u) {
        edgeMarker = xyz | 3u;
        EmitVertex();
        EndPrimitive();
    }
    if ((edgeFlags[0] & 2u) != 0u) {
        edgeMarker = xyz | 0u;
        EmitVertex();
        EndPrimitive();
    }
    if ((edgeFlags[0] & 4u) != 0u) {
        edgeMarker = xyz | 8u;
        EmitVertex();
        EndPrimitive();
    }
}