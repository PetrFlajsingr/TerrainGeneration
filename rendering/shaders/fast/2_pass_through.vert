#version 430

in uvec3 inCoords;
out uvec3 chunkCoords;

void main() {
    chunkCoords = inCoords;
}