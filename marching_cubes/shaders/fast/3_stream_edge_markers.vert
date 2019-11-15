#version 430

in uint caseMarker;
out uint edgeFlags;

uint getBit(uint val, uint pos) {
    return (val >> pos) & 1u;
}

void main() {
    uint cubeCase = caseMarker & 0xFFu;

    edgeFlags = caseMarker & 0xFFFFFF00u;
    uint bit0 = getBit(cubeCase, 0);
    if (getBit(cubeCase, 3) - bit0 != 0) {
        edgeFlags |= 1u;
    }
    if (getBit(cubeCase, 1) - bit0 != 0) {
        edgeFlags |= 2u;
    }
    if (getBit(cubeCase, 4) - bit0 != 0) {
        edgeFlags |= 4u;
    }
}