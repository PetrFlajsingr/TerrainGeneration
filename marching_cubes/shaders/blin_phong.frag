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

    FragColor = vec4(vec3(1, 0.5, 0) * colorLinear, 1.0);
    //FragColor = /*vec4(1,1,1,1);*/ vec4(colorForHeight(posi.y -5, 0, 30)*colorLinear, 1.0);
}