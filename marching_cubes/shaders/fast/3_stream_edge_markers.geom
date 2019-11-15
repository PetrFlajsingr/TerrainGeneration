#version 430

layout(points) in;
layout(points, max_vertices = 3) out;

in uint edgeFlags[];
out uint edgeMarker;

uint getBit(uint val, uint pos) {
    return (val >> pos) & 1u;
}

void main() {
    uint xyz = edgeFlags[0] & 0xFFFFFF00u;

    if (getBit(edgeFlags[0], 0) != 0u) {
        edgeMarker = xyz | 3u;
        EmitVertex();
        EndPrimitive();
    }
    if (getBit(edgeFlags[0], 1) != 0u) {
        edgeMarker = xyz | 0u;
        EmitVertex();
        EndPrimitive();
    }
    if (getBit(edgeFlags[0], 2) != 0u) {
        edgeMarker = xyz | 8u;
        EmitVertex();
        EndPrimitive();
    }
}