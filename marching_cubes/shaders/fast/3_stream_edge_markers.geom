#version 430

layout(binding=0, std430)buffer Vertices{ vec4 vertices[]; };
layout(binding=1, std430)buffer Density{ float density[]; };

layout(points) in;
layout(points, max_vertices = 3) out;

in uint caseMarker;
out uint edgeMarker;

void main() {
    uint xyz = caseMarker & 0xFFFFFF00u;

    if ((caseMarker & 0x1u) != 0) {
        edgeMarker = xyz | 0x3u;
        EmitVertex();
        EndPrimitive();
    }
    if ((caseMarker & 0x2u) != 0) {
        edgeMarker = xyz | 0x0u;
        EmitVertex();
        EndPrimitive();
    }
    if ((caseMarker & 0x4u) != 0) {
        edgeMarker = xyz | 0x8u;
        EmitVertex();
        EndPrimitive();
    }
}