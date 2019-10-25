#version 430
layout(location=0)in vec4 Position;
layout(location=1)in vec3 Normal;
layout(location=2)uniform mat4 mvpUniform;
layout(location=3)uniform vec3 lightPos;
layout(location=4)uniform vec3 cameraPos;

const vec3 lightColor = vec3(1, 1, 1);

out gl_PerVertex
{
    vec4 gl_Position;
};
out vec3 light;

void main() {
    gl_Position = mvpUniform * Position;

    float specularStrength = 0.8;

    vec3 ambient = 0.15 * lightColor;
    vec3 lightDir = normalize(lightPos - Position.xyz);

    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(cameraPos - Position.xyz);
    vec3 reflectDir = reflect(-lightDir, Normal);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    light = ambient + diffuse + specular;
}