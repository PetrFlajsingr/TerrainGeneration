#version 430
#extension GL_EXT_texture_array : enable

uniform mat4 view;
uniform mat4 inverseViewMatrix;
uniform mat4 lightViewProjectionMatrices[4];
uniform vec4 cascadedSplits;
uniform vec3 lightDir;
uniform int numOfCascades;
uniform float screenWidth;
uniform float screenHeight;

uniform sampler2DArrayShadow cascadedDepthTexture;

uniform sampler2D texturePlusX;
uniform sampler2D texturePlusY;
uniform sampler2D texturePlusZ;
uniform sampler2D textureMinusX;
uniform sampler2D textureMinusY;
uniform sampler2D textureMinusZ;

in vec3 v2fNormal;
in vec3 v2fPosition;
in vec3 FragPos;

out vec4 color;

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

vec3 readShadowMap(vec3 lightDirection, vec3 normal, float depthViewSpace, vec3 viewPosition) {
    float positiveViewSpaceZ = depthViewSpace;
    uint cascadeIdx = 0;

    for (uint i = 0; i < numOfCascades - 1; ++i)
    {
        if (positiveViewSpaceZ < cascadedSplits[i])
        {
            cascadeIdx = i + 1;
        }
    }

    float angleBias = 0.006f * (cascadeIdx);

    mat4 lightViewProjectionMatrix = lightViewProjectionMatrices[cascadeIdx];

    vec2 TexCoord = gl_FragCoord.xy / vec2(1920, 1080);
    vec4 tex_coord = vec4(TexCoord.x, TexCoord.y, cascadeIdx, 1.0);
    vec4 fragmentModelViewPosition = vec4(viewPosition, 1.0f);
    vec4 fragmentModelPosition = inverseViewMatrix * fragmentModelViewPosition;
    vec4 fragmentShadowPosition = lightViewProjectionMatrix * fragmentModelPosition;
    vec3 projCoords = fragmentShadowPosition.xyz /= fragmentShadowPosition.w;

    projCoords = projCoords * 0.5f + 0.5f;

    float currentDepth = projCoords.z;
    projCoords.z = cascadeIdx;

    float bias = max(angleBias * (1.0 - dot(normal, lightDirection)), 0.0008);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(cascadedDepthTexture, 0).xy;

    const int kernelSize = 5;
    const int kernelOffset = kernelSize / 2;
    for (int x = -kernelOffset; x <= kernelOffset; ++x)
    {
        for (int y = -kernelOffset; y <= kernelOffset; ++y)
        {
            vec3 coord = vec3(projCoords.xy + vec2(x, y) * texelSize, projCoords.z);
            float pcfDepth = shadow2DArray(cascadedDepthTexture, vec4(coord, currentDepth - bias)).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= float(kernelSize * kernelSize);

    return vec3(shadow);
}

vec3 chessBoard(vec3 pos) {

    pos /= 5000;
    const float xPlusWeight = float(v2fNormal.x > 0) * v2fNormal.x;
    const float yPlusWeight = float(v2fNormal.y > 0) * v2fNormal.y;
    const float zPlusWeight = float(v2fNormal.z > 0) * v2fNormal.z;
    const float xMinusWeight = float(v2fNormal.x < 0) * v2fNormal.x;
    const float yMinusWeight = float(v2fNormal.y < 0) * v2fNormal.y;
    const float zMinusWeight = float(v2fNormal.z < 0) * v2fNormal.z;

    return xPlusWeight * texture(texturePlusX, pos.yz).rgb
    + xMinusWeight * texture(textureMinusX, pos.yz).rgb
    + yPlusWeight * texture(texturePlusY, pos.xz).rgb
    + yMinusWeight * texture(textureMinusY, pos.xz).rgb
    + zPlusWeight * texture(texturePlusZ, pos.xy).rgb
    + zMinusWeight * texture(textureMinusZ, pos.xy).rgb;
}

float calculateFog() {
    const float distance = abs(v2fPosition.z);
    const float fogStart = 0;
    const float fogEnd = 100000;

    float fogFactor = (fogEnd - distance) / (fogEnd - fogStart);
    fogFactor = clamp(fogFactor, 0, 1);
    return fogFactor;
}


vec4 calculateDirectionalLight(vec3 viewPosition, vec3 viewNormal, vec3 lightDirection)
{
    lightDirection = -lightDirection;
    vec3 color = chessBoard(FragPos);
    vec3 normal = normalize(viewNormal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    float diff = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(-viewPosition + FragPos);
    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDirection + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    // calculate shad
    vec3 negatedLightDirection = lightDirection*-1.0f;
    vec3 lighting = (ambient + (1.0 - readShadowMap(negatedLightDirection, viewNormal, v2fPosition.z, viewPosition)) * (diffuse + specular)) * color;
    return vec4(lighting, 1.0);
}

void main() {
    vec3 lightDirection = normalize(vec3(lightDir));
    vec4 col = calculateDirectionalLight(v2fPosition.xyz, v2fNormal, lightDirection);

    const float f = calculateFog();
    color = (1 - f) * vec4(0.5, 0.5, 0.5, 1.0) + f * col;
}