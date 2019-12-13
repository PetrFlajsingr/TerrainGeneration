#version 430

const int NUM_CASCADES = 3;

in vec3 Normal0;
in vec3 WorldPos0;
in vec4 LightSpacePos[NUM_CASCADES];
in float clipSpacePos;

out vec4 FragColor;

uniform sampler2D shadowMap[NUM_CASCADES];
uniform float cascadeEnd[NUM_CASCADES];
uniform vec3 lightPos;
uniform vec3 viewPos;

float CalcShadowFactor(int CascadeIndex, vec4 LightSpacePos)
{
    vec3 ProjCoords = LightSpacePos.xyz / LightSpacePos.w;

    vec2 UVCoords;
    UVCoords.x = 0.5 * ProjCoords.x + 0.5;
    UVCoords.y = 0.5 * ProjCoords.y + 0.5;

    float z = 0.5 * ProjCoords.z + 0.5;

    vec3 lightDir = normalize(lightPos - WorldPos0);
    float bias = max(0.05 * (1.0 - dot(Normal0, lightDir)), 0.005);
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap[CascadeIndex], 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap[CascadeIndex], UVCoords.xy + vec2(x, y) * texelSize).r;
            shadow += z - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if(z > 1.0)
        shadow = 0.0;

    return shadow;
}

vec3 chessBoard(vec3 pos) {
    uint x = uint(abs(floor(pos.x)));
    uint y = uint(abs(floor(pos.y)));
    uint z = uint(abs(floor(pos.z)));

    if (y >=300) return vec3(1, 0, 0);
    if (x/10 % 2 == 0 ^^ z/10 % 2 == 0) {
        return vec3(0.5, 0, 0);
    }
    return vec3(0, 0.5, 0);
}

void main()
{
    float ShadowFactor = 0.0;
    vec3 c = vec3(0.f, 0.f, 0.f);
    for (int i = 0; i < NUM_CASCADES; i++) {
        if (clipSpacePos <= cascadeEnd[i]) {
            ShadowFactor = CalcShadowFactor(i, LightSpacePos[i]);
            if (i == 0) {
                c = vec3(0.5, 0.f, 0.f);
            } else if (i == 1) {
                c = vec3(0.f, 0.5f, 0.f);
            } else if (i == 2) {
                c = vec3(0.f, 0.f, 0.5f);
            }
            break;
        }
    }

    vec3 color = chessBoard(WorldPos0);
    vec3 normal = normalize(Normal0);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - WorldPos0);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - WorldPos0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    // calculate shad
    vec3 lighting = (ambient + (1.0 - ShadowFactor) * (diffuse + specular)) * (color + c);

    FragColor = vec4(lighting, 1.0);
}