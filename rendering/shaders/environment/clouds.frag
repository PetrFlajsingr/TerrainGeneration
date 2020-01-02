#version 430

in vec3 Pos;
in vec3 Normal;
out vec4 color;

uniform float time;

float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

float noise(vec3 x) {
    const vec3 step = vec3(110, 241, 171);

    vec3 i = floor(x);
    vec3 f = fract(x);

    float n = dot(i, step);

    vec3 u = f * f * (3.0 - 2.0 * f);
    return mix(mix(mix(hash(n + dot(step, vec3(0, 0, 0))), hash(n + dot(step, vec3(1, 0, 0))), u.x),
    mix(hash(n + dot(step, vec3(0, 1, 0))), hash(n + dot(step, vec3(1, 1, 0))), u.x), u.y),
    mix(mix(hash(n + dot(step, vec3(0, 0, 1))), hash(n + dot(step, vec3(1, 0, 1))), u.x),
    mix(hash(n + dot(step, vec3(0, 1, 1))), hash(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z);
}


//	Simplex 3D Noise
//	by Ian McEwan, Ashima Arts
//
vec4 permute(vec4 x){ return mod(((x*34.0)+1.0)*x, 289.0); }
vec4 taylorInvSqrt(vec4 r){ return 1.79284291400159 - 0.85373472095314 * r; }

float simplexNoise(vec3 v){
    const vec2  C = vec2(1.0/6.0, 1.0/3.0);
    const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

    // First corner
    vec3 i  = floor(v + dot(v, C.yyy));
    vec3 x0 =   v - i + dot(i, C.xxx);

    // Other corners
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min(g.xyz, l.zxy);
    vec3 i2 = max(g.xyz, l.zxy);

    //  x0 = x0 - 0. + 0.0 * C
    vec3 x1 = x0 - i1 + 1.0 * C.xxx;
    vec3 x2 = x0 - i2 + 2.0 * C.xxx;
    vec3 x3 = x0 - 1. + 3.0 * C.xxx;

    // Permutations
    i = mod(i, 289.0);
    vec4 p = permute(permute(permute(
    i.z + vec4(0.0, i1.z, i2.z, 1.0))
    + i.y + vec4(0.0, i1.y, i2.y, 1.0))
    + i.x + vec4(0.0, i1.x, i2.x, 1.0));

    // Gradients
    // ( N*N points uniformly over a square, mapped onto an octahedron.)
    float n_ = 1.0/7.0;// N=7
    vec3  ns = n_ * D.wyz - D.xzx;

    vec4 j = p - 49.0 * floor(p * ns.z *ns.z);//  mod(p,N*N)

    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_);// mod(j,N)

    vec4 x = x_ *ns.x + ns.yyyy;
    vec4 y = y_ *ns.x + ns.yyyy;
    vec4 h = 1.0 - abs(x) - abs(y);

    vec4 b0 = vec4(x.xy, y.xy);
    vec4 b1 = vec4(x.zw, y.zw);

    vec4 s0 = floor(b0)*2.0 + 1.0;
    vec4 s1 = floor(b1)*2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));

    vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy;
    vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww;

    vec3 p0 = vec3(a0.xy, h.x);
    vec3 p1 = vec3(a0.zw, h.y);
    vec3 p2 = vec3(a1.xy, h.z);
    vec3 p3 = vec3(a1.zw, h.w);

    //Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    vec4 m = max(0.6 - vec4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0);
    m = m * m;
    return 42.0 * dot(m*m, vec4(dot(p0, x0), dot(p1, x1),
    dot(p2, x2), dot(p3, x3)));
}

vec3 normalNoise(vec2 _st, float _zoom, float _speed){
    vec2 v1 = _st;
    vec2 v2 = _st;
    vec2 v3 = _st;
    float expon = pow(10.0, _zoom*2.0);
    v1 /= 1.0*expon;
    v2 /= 0.62*expon;
    v3 /= 0.83*expon;
    float n = time*_speed;
    float nr = (simplexNoise(vec3(v1, n)) + simplexNoise(vec3(v2, n)) + simplexNoise(vec3(v3, n))) / 6.0 + 0.5;
    n = time * _speed + 1000.0;
    float ng = (simplexNoise(vec3(v1, n)) + simplexNoise(vec3(v2, n)) + simplexNoise(vec3(v3, n))) / 6.0 + 0.5;
    return vec3(nr, ng, 0.5);
}

float cloudNoise(vec3 Position) {
    return simplexNoise(Position / 5) * 5
    + simplexNoise(Position / 20) * 20
    + simplexNoise(Position / 2) * 2
    ;
}

vec3 calcNormal(vec3 Position) {
    Position.y = 0;
    const float d = 100;
    vec3 grad;
    grad.x = cloudNoise(Position.xyz + vec3(d, 0, 0)) - cloudNoise(Position.xyz + vec3(-d, 0, 0));
    grad.y = cloudNoise(Position.xyz + vec3(0, d, 0)) - cloudNoise(Position.xyz + vec3(0, -d, 0));
    grad.z = cloudNoise(Position.xyz + vec3(0, 0, d)) - cloudNoise(Position.xyz + vec3(0, 0, -d));

    return normalize(grad);
}

void main() {
    vec3 tmp = Pos/3000;
    tmp.x += time;
    tmp.y -= time/20;

    const vec3 n = calcNormal(tmp);


    vec3 lightDirection = normalize(vec3(0, -1, 0));
    vec3 objColor = vec3(0.5);
    vec3 normal = n;
    vec3 lightColor = vec3(0.9);
    // ambient
    vec3 ambient = 0 * objColor;
    // diffuse
    float diff = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // calculate shad
    vec3 negatedLightDirection = lightDirection*-1.0f;
    vec3 lighting = (ambient + (diffuse)) * objColor;

    color = vec4(lighting, lighting.r);
}