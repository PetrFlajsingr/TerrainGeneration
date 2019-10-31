#version 430

layout(binding=0, std430)buffer VertexIDs{ uint vertexIDs[]; };


in uint edgeMarker;
out uint vertexID;

void setVertexID(uvec3 index, uint offset, uint vertexID) {
    uint dim = 32;
    vertexIDs[index.x + index.y * dim + index.z * dim * dim + offset] = vertexID;
}

void main() {
    uvec3 chunkCoord = uvec3(((edgeMarker >> 8u) & 0xFFu) * 3,
                       (edgeMarker >> 16u) & 0xFFu,
                       (edgeMarker >> 24u) & 0xFFu);
    uint edge = edgeMarker & 0xFFu;
    uint offset = 0;
    if (edge == 0) {
        offset = 1;
    } else if (edge == 8) {
        offset = 2;
    }

    setVertexID(chunkCoord, offset, gl_VertexID);
}