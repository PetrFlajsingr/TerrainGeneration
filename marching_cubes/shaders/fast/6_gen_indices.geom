#version 430

layout(binding=0, std430)buffer Density{ float density[]; };
layout(binding=1, std430)buffer PolyCountLUT{ uint polyCountLUT[]; };// length = cube count
layout(binding=2, std430)buffer PolyEdgesLUT{ uint polyEdgesLUT[]; };// length = cube count * 5 * 3 -- 3 edges per polygon, 5 polygons max
layout(binding=3, std430)buffer EdgeToVertexIdsLUT{ uvec2 edgeToVertexIdsLUT[]; };
layout(binding=4, std430)buffer VertexIDs{ uvec3 vertexIDs[]; };



layout(points) in;
layout(points, max_vertices = 15) out;

in uint cubeMarker;
out uvec3 indices;

uvec3 offsetForEdge(uint edge) {
    switch (edge) {
        case 3: return uvec3(0, 0, 0);
        case 0: return uvec3(1, 0, 0);
        case 8: return uvec3(2, 0, 0);

        case 1: return uvec3(0, 1, 0);
        case 9: return uvec3(2, 1, 0);

        case 4: return uvec3(1, 0, 1);
        case 7: return uvec3(0, 0, 1);

        case 11: return uvec3(3 + 2, 0, 0);
        case 2: return uvec3(3 + 1, 0, 0);

        case 5: return uvec3(0, 1, 1);

        case 6: return uvec3(3 + 1, 0, 1);

        case 10: return uvec3(3 + 2, 1, 0);
    }
    return uvec3(0, 0, 0);
}

void main() {
    uint dim = 32;
    uint caseIndex = cubeMarker & 0xFFu;
    uint polyCount = polyCountLUT[caseIndex];
    uvec3 chunkCoord = uvec3(((cubeMarker >> 8u) & 0xFFu) * 3,
                       (cubeMarker >> 16u) & 0xFFu,
                       (cubeMarker >> 24u) & 0xFFu);

    for (uint i = 0; i < polyCount; ++i) {
        uvec3 edgesForPoly = uvec3(polyEdgesLUT[caseIndex * 15], polyEdgesLUT[caseIndex * 15 + 1], polyEdgesLUT[caseIndex * 15 + 2]);

        uvec3 vertexIDCoord = edgesForPoly;
        vertexIDCoord.x *= 3;
        vertexIDCoord += offsetForEdge(edgesForPoly.x);
        indices.x = vertexIDs[vertexIDCoord.x + vertexIDCoord.y * dim + vertexIDCoord.z * dim * dim + offset];

        vertexIDCoord = edgesForPoly;
        vertexIDCoord.x *= 3;
        vertexIDCoord +=offsetForEdge(edgesForPoly.y);
        indices.y = vertexIDs[vertexIDCoord.x + vertexIDCoord.y * dim + vertexIDCoord.z * dim * dim + offset];

        vertexIDCoord = edgesForPoly;
        vertexIDCoord.x *= 3;
        vertexIDCoord += offsetForEdge(edgesForPoly.z);
        indices.z = vertexIDs[vertexIDCoord.x + vertexIDCoord.y * dim + vertexIDCoord.z * dim * dim + offset];

        EmitVertex();
        EndPrimitive();
    }
}