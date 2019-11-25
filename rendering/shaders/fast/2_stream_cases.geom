#version 430

layout(binding=0, std430)buffer Density{ float density[]; };

layout(points) in;
layout(points, max_vertices = 1) out;

in uvec3 chunkCoords[];

out uint marker;

uint getVertexStateFlag(bool state, uint position) {
    return uint(state) << position;
}

uint getCubeIndex(uint index) {
    uint dim = 32;
    float limit = 0;

    uint result = getVertexStateFlag(density[index + dim] > limit, 0);
    result |= getVertexStateFlag(density[index] > limit, 1);
    result |= getVertexStateFlag(density[index + 1] > limit, 2);
    result |= getVertexStateFlag(density[index + dim + 1] > limit, 3);
    result |= getVertexStateFlag(density[index + dim * dim + dim] > limit, 4);
    result |= getVertexStateFlag(density[index + dim * dim] > limit, 5);
    result |= getVertexStateFlag(density[index + dim * dim + 1] > limit, 6);
    result |= getVertexStateFlag(density[index + dim * dim + dim + 1] > limit, 7);


    result &= 0xFF * uint((index % dim) < dim - 1);
    result &= 0xFF * uint((index % (dim * dim)) < dim * (dim - 1));
    result &= 0xFF * uint((index % (dim * dim * dim)) < dim * dim * (dim - 1));

    /* if ((index % dim) >= dim - 1 || (index % (dim * dim)) >= dim * (dim - 1) || (index % (dim * dim * dim)) >= dim * dim * (dim - 1)) {
         result &= 1;
     }*/
    return result;
}

void main() {
    uint cubeIndex = getCubeIndex(gl_PrimitiveIDIn);
    if (cubeIndex != 0 && cubeIndex != 255) {
        uint dim = 32;
        uint result = cubeIndex;
        uint z = uint(gl_PrimitiveIDIn / (dim * dim) % dim);
        uint y = uint(gl_PrimitiveIDIn / dim % dim);
        uint x = uint(gl_PrimitiveIDIn % dim);
        result |= (x << 8u);
        result |= (y << 16u);
        result |= (z << 24u);
        marker = result;

        EmitVertex();
        EndPrimitive();
    }
}