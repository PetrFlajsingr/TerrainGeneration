#version 430
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 projection;
uniform mat4 modelView;
uniform mat4 lightSpaceMatrix;

void main()
{
    vs_out.FragPos = aPos.xyz;
    vs_out.Normal = aNormal;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = projection * modelView * aPos, 1.0;
}