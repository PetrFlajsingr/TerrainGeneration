#version 430
out vec4 FragColor;

in vec3 light;
uniform vec4 color;
void main()
{
    FragColor = vec4(color.xyz * light, color[3]);
}