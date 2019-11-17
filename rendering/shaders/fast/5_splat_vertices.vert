#version 430

layout(binding=0, std430)buffer VertexIDs{ uint vertexIDs[]; };

in uint edgeMarker;

void setVertexID(uvec3 index, uint vertexID) {
    const uint xDim = 32 * 3;
    const uint dim = 32;
    const uint arrayIndex = index.x + index.y * xDim + index.z * xDim * dim;
    vertexIDs[arrayIndex] = vertexID;
}

void main() {
    uvec3 chunkCoord = uvec3(((edgeMarker >> 8u) & 0xFFu),
                       (edgeMarker >> 16u) & 0xFFu,
                       (edgeMarker >> 24u) & 0xFFu);
    uint edge = edgeMarker & 0xFFu;
    chunkCoord.x *= 3;
    if (edge == 3) {
        chunkCoord.x += 0;
    } else if (edge == 0) {
        chunkCoord.x += 1;
    } else if (edge == 8) {
        chunkCoord.x += 2;
    }

    setVertexID(chunkCoord, gl_VertexID);
}