#version 430

in uint caseMarker;
out uint edgeFlags;

uint getBit(uint val, uint pos) {
    return (val >> pos) & 1u;
}

uint setBit(uint val, uint pos) {
    return val | (1u << pos);
}

void main() {
    uint cubeCase = caseMarker & 0xFFu;
    uvec3 build_vert_on_edge = abs(ivec3(getBit(cubeCase, 3), getBit(cubeCase, 1), getBit(cubeCase, 4)) - getBit(cubeCase, 0));

    edgeFlags = caseMarker & 0xFFFFFF00u;
    if (build_vert_on_edge.x != 0) {
        edgeFlags = setBit(edgeFlags, 0);
    }
    if (build_vert_on_edge.y != 0) {
        edgeFlags = setBit(edgeFlags, 1);
    }
    if (build_vert_on_edge.z != 0) {
        edgeFlags = setBit(edgeFlags, 2);
    }
}