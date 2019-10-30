#version 430

layout(binding=0, std430)buffer EdgeToVertexIdsLUT{ uvec2 edgeToVertexIdsLUT[]; };

in uint edgeMarker;

out vec4 Position;
out vec3 Normal;

uniform vec3 start;
uniform float step;

vec3 vertexForIndex(uint number, vec3 v0, float step) {
    uint w = 32;
    uint h = w;
    switch (number) {
        case 0: return v0 + vec3(0, step, 0);//offset + w;
        case 1: return v0;
        case 2: return v0 + vec3(step, 0, 0);// + 1;
        case 3: return v0 + vec3(step, step, 0);// + w + 1;
        case 4: return v0 + vec3(0, step, step);// + w * h + w;
        case 5: return v0 + vec3(0, 0, step);// + w * h;
        case 6: return v0 + vec3(step, 0, step);// + w * h + 1;
        case 7: return v0 + vec3(step, step, step);// + w * h + w + 1;
    }
    return vec3(0);
}

vec3 getPosition() {
    return start + gl_GlobalInvocationID * vec3(step);
}

uint getIndex() {
    uint dim = 32;
    return gl_GlobalInvocationID.z * dim * dim +
    gl_GlobalInvocationID.y * dim +
    gl_GlobalInvocationID.x;
}

float calculateDensity(vec3 vertex) {
    float hard_floor_y = 0.1;
    float rad = 3;
    float result = rad - length(vertex - vec3(0, -rad, 0));
    result += fbm(vertex.xyz);
    return result;
}

void main() {
    uvec3 chunkCoord = uvec3((edgeMarker >> 8u) & 0xFFu,
                             (edgeMarker >> 16u) & 0xFFu,
                             (edgeMarker >> 24u) & 0xFFu);
    uint edge = edgeMarker & 0xFFu;

    vec3 vecCoordInChunk = start + chunkCoord * vec3(step);

    vec3 v0 = vertexForIndex(edgeToVertexIdsLUT[edge].x, vecCoordInChunk, step);
    vec3 v1 = vertexForIndex(edgeToVertexIdsLUT[edge].y, vecCoordInChunk, step);

    vec3 diff = (v1 - v0);
    float perc = density[computeVertexIndex(edgeToVertexIdsLUT[edge].x, gl_PrimitiveIDIn)]
                    / abs(density[computeVertexIndex(edgeToVertexIdsLUT[edge].x, gl_PrimitiveIDIn)]
                    - density[computeVertexIndex(edgeToVertexIdsLUT[edge].y, gl_PrimitiveIDIn)]);

    Position = v0 + diff * abs(perc);

    float d = 1.0/(256);
    grad.x = calculateDensity(Position.xyz + vec3(d, 0, 0)) - calculateDensity(Position.xyz + vec3(-d, 0, 0));
    grad.y = calculateDensity(Position.xyz + vec3(0, d, 0)) - calculateDensity(Position.xyz + vec3(0, -d, 0));
    grad.z = calculateDensity(Position.xyz + vec3(0, 0, d)) - calculateDensity(Position.xyz + vec3(0, 0, -d));


    Normal = -normalize(grad);
}