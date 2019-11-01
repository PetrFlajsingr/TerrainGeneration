#version 430

layout(points) in;
layout(points, max_vertices = 3) out;

in uint caseMarker[];

out uint edgeMarker;

bool hasVertex(uint marker, uint edge1, uint edge2) {
    return ((((marker & edge1) != 0) ^^ ((marker & edge2) != 0)));
}

void main() {
    uint xyz = caseMarker[0] & 0xFFFFFF00u;

    if (hasVertex(caseMarker[0], 0x1u, 0x1u << 3)) {
        edgeMarker = xyz | 0x3u;
        EmitVertex();
        EndPrimitive();
    }

    if (hasVertex(caseMarker[0], 0x1u, 0x1u << 1)) {
        edgeMarker = xyz | 0x0u;
        EmitVertex();
        EndPrimitive();
    }

    if (hasVertex(caseMarker[0], 0x1u, 0x1u << 4)) {
        edgeMarker = xyz | 0x8u;
        EmitVertex();
        EndPrimitive();
    }
}