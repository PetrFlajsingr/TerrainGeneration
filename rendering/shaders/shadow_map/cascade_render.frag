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

in vec3 v2fNormal;
in vec3 v2fPosition;
in vec3 FragPos;

out vec4 color;

vec3 readShadowMap(vec3 lightDirection, vec3 normal, float depthViewSpace, vec3 viewPosition)
{

    float positiveViewSpaceZ = depthViewSpace;
    uint cascadeIdx = 0;

    for (uint i = 0; i < numOfCascades - 1; ++i)
    {
        if (positiveViewSpaceZ < cascadedSplits[i])
        {
            cascadeIdx = i + 1;
        }
    }

    float angleBias = 0.006f * (cascadeIdx + 1);

    mat4 lightViewProjectionMatrix = lightViewProjectionMatrices[cascadeIdx];

    vec2 TexCoord = gl_FragCoord.xy / vec2(1920, 1080);
    vec4 tex_coord = vec4(TexCoord.x, TexCoord.y, cascadeIdx, 1.0);

    vec4 fragmentModelViewPosition = vec4(viewPosition, 1.0f);

    vec4 fragmentModelPosition = inverseViewMatrix * fragmentModelViewPosition;

    vec4 fragmentShadowPosition = lightViewProjectionMatrix * fragmentModelPosition;

    vec3 projCoords = fragmentShadowPosition.xyz /= fragmentShadowPosition.w;

    //Remap the -1 to 1 NDC to the range of 0 to 1
    projCoords = projCoords * 0.5f + 0.5f;

    // Get depth of current fragment from light's perspective
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
    uint x = uint(abs(floor(pos.x)));
    uint y = uint(abs(floor(pos.y)));
    uint z = uint(abs(floor(pos.z)));

    if (x/10 % 2 == 0 ^^ z/10 % 2 == 0) {
        return vec3(0.5, 0, 0);
    }
    return vec3(0, 0.5, 0);
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
    vec3 viewDir = normalize(viewPosition - FragPos);
    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDirection + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    // calculate shad
    vec3 negatedLightDirection = lightDirection*-1.0f;
    vec3 lighting = (ambient + (1.0 - readShadowMap(negatedLightDirection, viewNormal, FragPos.z, viewPosition)) * (diffuse + specular)) * color;
    return vec4(lighting, 1.0);
}

void main()
{
    uint cascadeIdx = 0;

    for (uint i = 0; i < numOfCascades - 1; ++i)
    {
        if (v2fPosition.z < cascadedSplits[i])
        {
            cascadeIdx = i + 1;
        }
    }

    vec3 lightDirection = normalize(vec3(lightDir));
    vec4 col = calculateDirectionalLight(v2fPosition.xyz, v2fNormal, lightDirection);

    vec4 c;
    if (cascadeIdx == 0) {
        c = vec4(0.2, 0, 0, 0);
    } else if (cascadeIdx == 1) {
        c = vec4(0, 0.2, 0, 0);
    } else if (cascadeIdx == 2) {
        c = vec4(0, 0, 0.2, 0);
    } else if (cascadeIdx == 3) {
        c = vec4(0.2, 0, 0, 0);
    }

    color = col + c;
}