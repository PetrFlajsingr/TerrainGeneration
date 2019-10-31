#version 430

layout(points) in;
layout(points, max_vertices = 3) out;

in uint caseMarker[];

out uint edgeMarker;

void main() {
  //  uint xyz = caseMarker[0] & 0xFFFFFF00u;

    edgeMarker = caseMarker[0];
    EmitVertex();
    EndPrimitive();
   /* if ((caseMarker[0] & 0x1u) != 0) {
        edgeMarker = xyz | 0x3u;
        EmitVertex();
        EndPrimitive();
    }
    if ((caseMarker[0] & 0x2u) != 0) {
        edgeMarker = xyz | 0x0u;
        EmitVertex();
        EndPrimitive();
    }
    if ((caseMarker[0] & 0x4u) != 0) {
        edgeMarker = xyz | 0x8u;
        EmitVertex();
        EndPrimitive();
    }*/
}