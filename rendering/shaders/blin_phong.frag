#version 430

in vec3 NormalInterp;
in vec3 Pos;

uniform vec4 color;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightPower;
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specColor;

uniform float shininess;

in vec3 posi;

out vec4 FragColor;

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


vec3 colorForHeight(float height, float minHeight, float maxHeight) {

    float range = maxHeight/2 - minHeight;
    vec3 color;
    if(height >= minHeight && height <= maxHeight/2) {
        color.y = height / range;
        color.x = 1.0f - color.y;
        color.z = 0.0f;

    } else if(height > maxHeight/2 && height <= maxHeight) {
        color.x = 0.0f;
        color.z = (height - maxHeight/2) / range;
        color.y = 1.0f - color.z;
    }
    return color;
}

vec3 chessBoard(vec3 pos) {
    /*if (pos.y < 100) {
        float val = (pos.y) / 50;
        return vec3(val, 0, 0);
    }
    if (pos.y < 220) {
        return vec3(0, (pos.y) / 150, 0);
    }*/
    uint x = uint(floor(pos.x));
    uint y = uint(floor(pos.y));
    uint z = uint(floor(pos.z));

    if (x/10 % 2 == 0 ^^ z/10 % 2 == 0) {
        return vec3(0.5, 0, 0);
    }
    return vec3(0, 0.5, 0);
}

void main() {

    vec3 normal = normalize(NormalInterp);
    vec3 lightDir = lightPos - Pos;
    float distance = length(lightDir);
    distance = distance * distance;
    lightDir = normalize(lightDir);

    float lambertian = max(dot(lightDir,normal), 0.0);
    float specular = 0.0;

    if(lambertian > 0.0) {
        vec3 viewDir = normalize(-Pos);

        vec3 halfDir = normalize(lightDir + viewDir);
        float specAngle = max(dot(halfDir, normal), 0.0);
        specular = pow(specAngle, shininess);

    }
    vec3 colorLinear = ambientColor/2 +
    diffuseColor * lambertian * lightColor * lightPower / distance +
    specColor * specular * lightColor * lightPower / distance;

    FragColor = vec4(chessBoard(posi) * colorLinear, 1.0);
    //FragColor = /*vec4(1,1,1,1);*/ vec4(colorForHeight(posi.y -5, 0, 30)*colorLinear, 1.0);
}