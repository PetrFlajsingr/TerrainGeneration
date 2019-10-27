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

in float yPos;

out vec4 FragColor;
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
    vec3 colorLinear = ambientColor +
    diffuseColor * lambertian * lightColor * lightPower / distance +
    specColor * specular * lightColor * lightPower / distance;

    vec3 color = vec3(0);
    if (yPos < 3.4) {
        color.z = (yPos - 2.8) * 1;
    } else if (yPos < 3.8) {
        color.y = (yPos - 3.4) * 1;
    } else {
        color =  vec3((yPos - 4)*1, (yPos - 3.4)*1, 0);
    }
    FragColor = vec4(color*colorLinear, 1.0);
}