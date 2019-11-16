#version 430

layout(binding=0, std430)buffer Density{ float density[]; };
layout(binding=1, std430)buffer PolyCountLUT{ uint polyCountLUT[]; };// length = cube count
layout(binding=2, std430)buffer PolyEdgesLUT{ uint polyEdgesLUT[]; };// length = cube count * 5 * 3 -- 3 edges per polygon, 5 polygons max
layout(binding=3, std430)buffer EdgeToVertexIdsLUT{ uvec2 edgeToVertexIdsLUT[]; };
layout(binding=4, std430)buffer VertexIDs{ uint vertexIDs[]; };

layout(points) in;
layout(points, max_vertices = 15) out;

in uint cubeMarker[];
out uvec3 indices;

uvec3 inChunkOffset(uint edge) {
    switch (edge) {
        case 3: //fallthrough
        case 0: //fallthrough
        case 8: return uvec3(0, 0, 0);
        case 4: //fallthrough
        case 7: return uvec3(0, 0, 1);
        case 1: //fallthrough
        case 9: return uvec3(0, 1, 0);
        case 5: return uvec3(0, 1, 1);
        case 11: //fallthrough
        case 2: return uvec3(1, 0, 0);
        case 6: return uvec3(1, 0, 1);
        case 10: return uvec3(1, 1, 0);
    }
    return uvec3(54645654, 456456456, 456456);
}

uint offsetForEdge(uint edge) {
    switch (edge) {
        case 1: //fallthrough
        case 3: //fallthrough
        case 5: //fallthrough
        case 7: return 0;

        case 0: //fallthrough
        case 2: //fallthrough
        case 4: //fallthrough
        case 6: return 1;

        case 8: //fallthrough
        case 9: //fallthrough
        case 10: //fallthrough
        case 11: return 2;
    }
    return 0;
}

uint getIndex(uint edge, uvec3 coord) {
    uint dim = 32;
    coord += inChunkOffset(edge);
    coord.x *= 3;
    coord.x += offsetForEdge(edge);
    return vertexIDs[coord.x + coord.y * dim + coord.z * dim * dim];
}

void main() {
    uint dim = 32;
    uint caseIndex = cubeMarker[0] & 0xFFu;
    uint polyCount = polyCountLUT[caseIndex];
    uvec3 chunkCoord = uvec3(((cubeMarker[0] >> 8u) & 0xFFu),
        (cubeMarker[0] >> 16u) & 0xFFu,
        (cubeMarker[0] >> 24u) & 0xFFu);
    if (max(max(chunkCoord.x, chunkCoord.y), chunkCoord.z) >= 5) {
        polyCount = 0;
    }

    for (uint i = 0; i < polyCount; ++i) {
        uvec3 edgesForPoly = uvec3(polyEdgesLUT[caseIndex * 15 + i*3], polyEdgesLUT[caseIndex * 15 + i*3 + 1], polyEdgesLUT[caseIndex * 15 + i*3 + 2]);

        indices.x = getIndex(edgesForPoly.x, chunkCoord);
        indices.y = getIndex(edgesForPoly.y, chunkCoord);
        indices.z = getIndex(edgesForPoly.z, chunkCoord);


        if (true || indices.x * indices.y * indices.z != 0) {
            EmitVertex();
            EndPrimitive();
        }
    }
}