#version 430

layout(binding=0, std430)buffer EdgeToVertexIdsLUT{ uvec2 edgeToVertexIdsLUT[]; };
layout(binding=1, std430)buffer Density{ float density[]; };

in uint edgeMarker;

out vec4 Position;
out vec3 Normal;

uniform vec3 start;
uniform float chunkStep;

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

//	Classic Perlin 3D Noise
//	by Stefan Gustavson
//
vec4 permute(vec4 x){ return mod(((x*34.0)+1.0)*x, 289.0); }
vec4 taylorInvSqrt(vec4 r){ return 1.79284291400159 - 0.85373472095314 * r; }
vec3 fade(vec3 t) { return t*t*t*(t*(t*6.0-15.0)+10.0); }

float cnoise(vec3 P){
    vec3 Pi0 = floor(P);// Integer part for indexing
    vec3 Pi1 = Pi0 + vec3(1.0);// Integer part + 1
    Pi0 = mod(Pi0, 289.0);
    Pi1 = mod(Pi1, 289.0);
    vec3 Pf0 = fract(P);// Fractional part for interpolation
    vec3 Pf1 = Pf0 - vec3(1.0);// Fractional part - 1.0
    vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
    vec4 iy = vec4(Pi0.yy, Pi1.yy);
    vec4 iz0 = Pi0.zzzz;
    vec4 iz1 = Pi1.zzzz;

    vec4 ixy = permute(permute(ix) + iy);
    vec4 ixy0 = permute(ixy + iz0);
    vec4 ixy1 = permute(ixy + iz1);

    vec4 gx0 = ixy0 / 7.0;
    vec4 gy0 = fract(floor(gx0) / 7.0) - 0.5;
    gx0 = fract(gx0);
    vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
    vec4 sz0 = step(gz0, vec4(0.0));
    gx0 -= sz0 * (step(0.0, gx0) - 0.5);
    gy0 -= sz0 * (step(0.0, gy0) - 0.5);

    vec4 gx1 = ixy1 / 7.0;
    vec4 gy1 = fract(floor(gx1) / 7.0) - 0.5;
    gx1 = fract(gx1);
    vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
    vec4 sz1 = step(gz1, vec4(0.0));
    gx1 -= sz1 * (step(0.0, gx1) - 0.5);
    gy1 -= sz1 * (step(0.0, gy1) - 0.5);

    vec3 g000 = vec3(gx0.x, gy0.x, gz0.x);
    vec3 g100 = vec3(gx0.y, gy0.y, gz0.y);
    vec3 g010 = vec3(gx0.z, gy0.z, gz0.z);
    vec3 g110 = vec3(gx0.w, gy0.w, gz0.w);
    vec3 g001 = vec3(gx1.x, gy1.x, gz1.x);
    vec3 g101 = vec3(gx1.y, gy1.y, gz1.y);
    vec3 g011 = vec3(gx1.z, gy1.z, gz1.z);
    vec3 g111 = vec3(gx1.w, gy1.w, gz1.w);

    vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
    g000 *= norm0.x;
    g010 *= norm0.y;
    g100 *= norm0.z;
    g110 *= norm0.w;
    vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
    g001 *= norm1.x;
    g011 *= norm1.y;
    g101 *= norm1.z;
    g111 *= norm1.w;

    float n000 = dot(g000, Pf0);
    float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
    float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
    float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
    float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
    float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
    float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
    float n111 = dot(g111, Pf1);

    vec3 fade_xyz = fade(Pf0);
    vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
    vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
    float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);
    return 2.2 * n_xyz;
}



uniform uint octaves;
uniform float gain;
uniform float lacunarity;
uniform float sharpness;
uniform float valleyScale;
uniform float heightScale;


float nTest(vec3 x, uint numOctaves) {
    float result = 0;
    const float Ia = 1;
    const float If = 0.0000001;
    const float Ss = sharpness;
    const float Se = valleyScale;
    const float Sh = heightScale;
    float g = gain;
    float l = lacunarity;
    for (uint i = 0; i < numOctaves; ++i) {
        const float g = i * g;
        const float l = i * l;
        const float eps = Ia * g;
        const float lambda = If * l;

        const float perlinNoiseValue = eps * cnoise(x * lambda);

        float noiseValue = 0;
        if (Ss >= 0) {
            const float billowNoiseValue = abs(perlinNoiseValue);
            noiseValue = mix(perlinNoiseValue, billowNoiseValue, abs(Ss));
        } else {
            const float billowNoiseValue = abs(perlinNoiseValue);
            const float ridgeNoiseValue = 1.0f - billowNoiseValue;
            noiseValue = mix(perlinNoiseValue, ridgeNoiseValue, abs(Ss));
        }

        const float resultNoiseValue = noiseValue * pow(abs(noiseValue), Se) * Sh;

        result += resultNoiseValue;
    }
    return result;
}

float calculateDensity(vec3 vertex) {
    return -vertex.y + nTest(vertex, octaves);
}

void main() {
    uvec3 chunkCoord = uvec3((edgeMarker >> 8u) & 0xFFu,
    (edgeMarker >> 16u) & 0xFFu,
    (edgeMarker >> 24u) & 0xFFu);
    uint edge = edgeMarker & 0xFFu;

    vec3 vecCoordInChunk = start + chunkCoord * vec3(chunkStep);

    vec3 v0 = vertexForIndex(edgeToVertexIdsLUT[edge].x, vecCoordInChunk, chunkStep);
    vec3 v1 = vertexForIndex(edgeToVertexIdsLUT[edge].y, vecCoordInChunk, chunkStep);

    vec3 diff = (v1 - v0);
    uint v0Index = computeVertexIndex(chunkCoord, edgeToVertexIdsLUT[edge].x);
    uint v1Index = computeVertexIndex(chunkCoord, edgeToVertexIdsLUT[edge].y);
    float perc = density[v0Index] / abs(density[v0Index] - density[v1Index]);

    Position = vec4(v0 + diff * abs(perc), 1);

    float d = 1.0/(1);
    vec3 grad;
    grad.x = calculateDensity(Position.xyz + vec3(d, 0, 0)) - calculateDensity(Position.xyz + vec3(-d, 0, 0));
    grad.y = calculateDensity(Position.xyz + vec3(0, d, 0)) - calculateDensity(Position.xyz + vec3(0, -d, 0));
    grad.z = calculateDensity(Position.xyz + vec3(0, 0, d)) - calculateDensity(Position.xyz + vec3(0, 0, -d));

    Normal = -normalize(grad);
}