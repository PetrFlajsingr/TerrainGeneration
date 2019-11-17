#version 430

layout(binding=0, std430)buffer Density{ float density[]; };
layout(binding=1, std430)buffer PolyCountLUT{ uint polyCountLUT[]; };// length = cube count
layout(binding=2, std430)buffer PolyEdgesLUT{ uint polyEdgesLUT[]; };// length = cube count * 5 * 3 -- 3 edges per polygon, 5 polygons max
layout(binding=3, std430)buffer EdgeToVertexIdsLUT{ uvec2 edgeToVertexIdsLUT[]; };
layout(binding=4, std430)buffer VertexIDs{ uint vertexIDs[]; };

layout(points) in;
layout(points, max_vertices = 30) out;

in uint cubeMarker[];
out uvec3 indices;

ivec3 inChunkOffset(uint edge) {
    const int x = 1;
    const int y = -1;
    const int z = 1;
    switch (edge) {
        case 3: //fallthrough
        case 0: //fallthrough
        case 8: return ivec3(0, 0, 0);
        case 4: //fallthrough
        case 7: return ivec3(0, 0, z);
        case 1: //fallthrough
        case 9: return ivec3(0, y, 0);
        case 5: return ivec3(0, y, z);
        case 11: //fallthrough
        case 2: return ivec3(x, 0, 0);
        case 6: return ivec3(x, 0, z);
        case 10: return ivec3(x, y, 0);
    }
    return ivec3(0, 0, 0);
}

int offsetForEdge(uint edge) {
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
    const uint xDim = 32 * 3;
    const uint dim = 32;
    ivec3 tmp = ivec3(coord);
    tmp += inChunkOffset(edge);
    tmp.x *= 3;
    tmp.x += offsetForEdge(edge);
    return vertexIDs[tmp.x + tmp.y * xDim + tmp.z * xDim * dim];
}

void main() {
    uint dim = 32;
    uint caseIndex = cubeMarker[0] & 0xFFu;
    uint polyCount = polyCountLUT[caseIndex];
    uvec3 chunkCoord = uvec3(((cubeMarker[0] >> 8u) & 0xFFu),
        (cubeMarker[0] >> 16u) & 0xFFu,
        (cubeMarker[0] >> 24u) & 0xFFu);
    if (chunkCoord.x > 29 || chunkCoord.y < 1 || chunkCoord.z > 29) {
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