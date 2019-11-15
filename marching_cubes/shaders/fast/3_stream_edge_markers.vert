#version 430

in uint caseMarker;
out uint edgeFlags;

uint getBit(uint val, uint pos) {
    return (val >> pos) & 1u;
}

void main() {
    uint cubeCase = caseMarker & 0xFFu;
    uint  bit0 = (cubeCase      ) & 1u;
    uint  bit3 = (cubeCase >> 3u) & 1u;
    uint  bit1 = (cubeCase >> 1u) & 1u;
    uint  bit4 = (cubeCase >> 4u) & 1u;
    uvec3 build_vert_on_edge = abs(uvec3(bit3,bit1,bit4) - bit0);

    edgeFlags = caseMarker & 0xFFFFFF00u;
    if (build_vert_on_edge.x != 0)
        edgeFlags |= 1u;
    if (build_vert_on_edge.y != 0)
        edgeFlags |= 2u;
    if (build_vert_on_edge.z != 0)
        edgeFlags |= 4u;
}