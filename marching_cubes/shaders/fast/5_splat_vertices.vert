#version 430

layout(binding=0, std430)buffer VertexIDs{ uint vertexIDs[]; };


in uint edgeMarker;

void setVertexID(uvec3 index, uint offset, uint vertexID) {
    uint dim = 32;
    uint arrayIndex = index.x + index.y * dim + index.z * dim * dim;
    vertexIDs[arrayIndex + offset] = vertexID;
}

void main() {
    uvec3 chunkCoord = uvec3(((edgeMarker >> 8u) & 0xFFu) * 3,
                       (edgeMarker >> 16u) & 0xFFu,
                       (edgeMarker >> 24u) & 0xFFu);
    uint edge = edgeMarker & 0xFFu;
    uint offset = 0 + uint(edge == 0) * 1 + uint(edge == 8) * 2;

    setVertexID(chunkCoord, offset, gl_VertexID);
}