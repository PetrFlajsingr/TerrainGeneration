#version 430

layout(binding=0, std430)buffer EdgeToVertexIdsLUT{ uvec2 edgeToVertexIdsLUT[]; };
layout(binding=1, std430)buffer Density{ float density[]; };

in uint edgeMarker;

out vec4 Position;
out vec3 Normal;

uniform vec3 start;
uniform float step;

float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

float noise(vec3 x) {
    const vec3 step = vec3(110, 241, 171);

    vec3 i = floor(x);
    vec3 f = fract(x);

    float n = dot(i, step);

    vec3 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(mix( hash(n + dot(step, vec3(0, 0, 0))), hash(n + dot(step, vec3(1, 0, 0))), u.x),
    mix( hash(n + dot(step, vec3(0, 1, 0))), hash(n + dot(step, vec3(1, 1, 0))), u.x), u.y),
    mix(mix( hash(n + dot(step, vec3(0, 0, 1))), hash(n + dot(step, vec3(1, 0, 1))), u.x),
    mix( hash(n + dot(step, vec3(0, 1, 1))), hash(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z);
}

float fbm(vec3 x, uint numOctaves) {
    float v = 0.0;
    float a = 0.5;
    vec3 shift = vec3(100);
    for (int i = 0; i < numOctaves; ++i) {
        v += a * noise(x);
        x = x * 2.0 + shift;
        a *= 0.5;
    }
    return v;
}

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

uint computeVertexIndex(uvec3 chunkCoord, uint vertexNumber) {
    uint w = 32;
    uint h = w;
    uint index = chunkCoord.x + chunkCoord.y * w + chunkCoord.z * w * h;
    switch (vertexNumber) {
        case 0: return index + w;
        case 1: return index;
        case 2: return index + 1;
        case 3: return index + w + 1;
        case 4: return index + w * h + w;
        case 5: return index + w * h;
        case 6: return index + w * h + 1;
        case 7: return index + w * h + w + 1;
    }
    return 0;
}

float distToSphere(vec3 vertex, vec3 center, float radius) {
    return -(sqrt(pow(center.x-vertex.x, 2) + pow(center.y-vertex.y, 2) + pow(center.z-vertex.z, 2)) - radius);
}

float calculateDensity(vec3 vertex) {
    float hard_floor_y = 10;
    float rad = 3;
    float result = rad - length(vertex - vec3(0, -rad, 0));
    result += fbm(vertex.xyz, 5);

    vertex.y += 10;
    return -vertex.y
    + noise(vertex/20)*20
    + noise(vertex/10)*10
    + noise(vertex/2)*2
    + noise(vertex*8)/8;
   // return -vertex.y + (sin(vertex.x/5) + sin(vertex.z/5))*7 + 20;;
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
    uint v0Index = computeVertexIndex(chunkCoord, edgeToVertexIdsLUT[edge].x);
    uint v1Index = computeVertexIndex(chunkCoord, edgeToVertexIdsLUT[edge].y);
    float perc = density[v0Index] / abs(density[v0Index] - density[v1Index]);

    Position = vec4(v0 + diff * abs(perc), 1);

    float d = 1.0/(512);
    vec3 grad;
    grad.x = calculateDensity(Position.xyz + vec3(d, 0, 0)) - calculateDensity(Position.xyz + vec3(-d, 0, 0));
    grad.y = calculateDensity(Position.xyz + vec3(0, d, 0)) - calculateDensity(Position.xyz + vec3(0, -d, 0));
    grad.z = calculateDensity(Position.xyz + vec3(0, 0, d)) - calculateDensity(Position.xyz + vec3(0, 0, -d));

    Normal = -normalize(grad);
}