#version 430


layout(binding=0, std430)buffer Density{ float density[]; };
layout(binding=1, std430)buffer PolyCountLUT{ uint polyCountLUT[]; };// length = cube count
layout(binding=2, std430)buffer PolyEdgesLUT{ uint polyEdgesLUT[]; };// length = cube count * 5 * 3 -- 3 edges per polygon, 5 polygons max

struct uintPair { uint first; uint second; };
layout(binding=3, std430)buffer EdgeToVertexIdsLUT{ uintPair edgeToVertexIdsLUT[]; };
layout(binding=4, std430)buffer CubeIndex{ uint cubeIndex[]; };
layout(binding=5, std430)buffer Vertices{ vec4 vertices[]; };

layout(points) in;
layout(points, max_vertices = 15) out;

in gl_PerVertex
{
//uint gl_Position;
    vec4 gl_Position;
} gl_in[];

out Wat{
    vec4 pos;
    vec3 normal;
}data;


vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}
vec3 fade(vec3 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}

float cnoise(vec3 P){
    vec3 Pi0 = floor(P); // Integer part for indexing
    vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
    Pi0 = mod(Pi0, 289.0);
    Pi1 = mod(Pi1, 289.0);
    vec3 Pf0 = fract(P); // Fractional part for interpolation
    vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
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

    vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
    vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
    vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
    vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
    vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
    vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
    vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
    vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

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


float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

float noise(float x) {
    float i = floor(x);
    float f = fract(x);
    float u = f * f * (3.0 - 2.0 * f);
    return mix(hash(i), hash(i + 1.0), u);
}

float noise(vec2 x) {
    vec2 i = floor(x);
    vec2 f = fract(x);

    // Four corners in 2D of a tile
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    // Simple 2D lerp using smoothstep envelope between the values.
    // return vec3(mix(mix(a, b, smoothstep(0.0, 1.0, f.x)),
    //			mix(c, d, smoothstep(0.0, 1.0, f.x)),
    //			smoothstep(0.0, 1.0, f.y)));

    // Same code, with the clamps in smoothstep and common subexpressions
    // optimized away.
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

// This one has non-ideal tiling properties that I'm still tuning
float noise(vec3 x) {
    const vec3 step = vec3(110, 241, 171);

    vec3 i = floor(x);
    vec3 f = fract(x);

    // For performance, compute the base input to a 1D hash from the integer part of the argument and the
    // incremental change to the 1D based on the 3D -> 1D wrapping
    float n = dot(i, step);

    vec3 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(mix( hash(n + dot(step, vec3(0, 0, 0))), hash(n + dot(step, vec3(1, 0, 0))), u.x),
    mix( hash(n + dot(step, vec3(0, 1, 0))), hash(n + dot(step, vec3(1, 1, 0))), u.x), u.y),
    mix(mix( hash(n + dot(step, vec3(0, 0, 1))), hash(n + dot(step, vec3(1, 0, 1))), u.x),
    mix( hash(n + dot(step, vec3(0, 1, 1))), hash(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z);
}


float wtf(vec3 p, float size) {
    if (abs(p.x) < size) {
        return -abs(p.x);
    }
    if (abs(p.y) < size) {
        return -abs(p.y);
    }
    if (abs(p.z) < size) {
        return -abs(p.z);
    }
    return abs(p.x);
}

float cube(vec3 p, float size) {
    p -= size;
    if (abs(p.x) < size && abs(p.y) < size && abs(p.z) < size) {
        return -(abs(p.x) + abs(p.y) + abs(p.z));
    }
    return (abs(p.x) + abs(p.y) + abs(p.z));
}

uniform float offset;
float fnc(vec3 vertex) {
    float hard_floor_y = 0.1;
    //return -sqrt(vertex.y) +0.3+ sqrt(abs(vertex.x - 0.3));
    //return -vertex.y + 0.5;
    return cnoise(vertex.xyz + offset) + clamp((hard_floor_y - vertex.y) * 3 * 40, 0.0, 1.0)
    +noise(vertex.xyz*3 + offset) - 0.5;
    //return -vertex.y + 0.5;
    //return -(sqrt(pow(1-vertex.x, 2) + pow(1-vertex.y, 2) + pow(1-vertex.z, 2)) - 0.9);
    //return -(sqrt(pow(0.25-vertex.x*0.7, 2) + pow(.5-vertex.y, 2) + pow(.5-vertex.z, 2)) - 0.25);
    //return  vertex.x + -vertex.y + 0.1;
    //return -sin(vertex.y*10);
    //return cube(vertex.xyz, 0.96);
    //return -wtf(vertex.xyz, 0.5);
    //return  pow(vertex.x - 0.5, 2) / 10 + pow(vertex.z - 0.5, 2) / 5;
}

vec3 calculateNormal(vec3 p1, vec3 p2, vec3 p3) {
    vec3 vector1 = p2.xyz - p1.xyz;
    vec3 vector2 = p3.xyz - p1.xyz;
    return normalize (cross (vector1, vector2));
}

uint computeVertexIndex(uint number, uint offset) {
    uint w = 32;
    uint h = w;
    switch (number) {
        case 0: return offset + w;
        case 1: return offset;
        case 2: return offset + 1;
        case 3: return offset + w + 1;
        case 4: return offset + w * h + w;
        case 5: return offset + w * h;
        case 6: return offset + w * h + 1;
        case 7: return offset + w * h + w + 1;
    }
    return 0;
}


void main() {
    uint dim = 8;
    //if (gl_PrimitiveIDIn % 8 == 7 || gl_PrimitiveIDIn % dim * dim >= dim * (dim - 1) || gl_PrimitiveIDIn >= pow(dim, 3) - pow(dim, 2) - 6) return;
    uint local_cubeIndex = cubeIndex[gl_PrimitiveIDIn];
    for (uint i = 0; i < polyCountLUT[local_cubeIndex]; ++i) {
        float d = 1.0/(256);
        vec3 grad = vec3(0, -1, 0);
        vec4 polyVecs[3];
        uint edge = polyEdgesLUT[local_cubeIndex * 15];

        for (uint j = i * 3; j < (i * 3 + 3); ++j) {
            uint edge = polyEdgesLUT[local_cubeIndex * 15 + j];
            vec4 v0 = vertices[computeVertexIndex(edgeToVertexIdsLUT[edge].first, gl_PrimitiveIDIn)];
            vec4 v1 = vertices[computeVertexIndex(edgeToVertexIdsLUT[edge].second, gl_PrimitiveIDIn)];

            vec4 diff = (v1 - v0);
            float perc = density[computeVertexIndex(edgeToVertexIdsLUT[edge].first, gl_PrimitiveIDIn)]
                / abs(density[computeVertexIndex(edgeToVertexIdsLUT[edge].first, gl_PrimitiveIDIn)]
                    - density[computeVertexIndex(edgeToVertexIdsLUT[edge].second, gl_PrimitiveIDIn)]);

            data.pos = v0 + diff * abs(perc);
            grad.x = fnc(data.pos.xyz + vec3(d, 0, 0)) - fnc(data.pos.xyz + vec3(-d, 0, 0));
            grad.y = fnc(data.pos.xyz + vec3(0, d, 0)) - fnc(data.pos.xyz + vec3(0, -d, 0));
            grad.z = fnc(data.pos.xyz + vec3(0, 0, d)) - fnc(data.pos.xyz + vec3(0, 0, -d));


            data.normal = -normalize(grad);

            EmitVertex();
            EndPrimitive();
        }
    }

}