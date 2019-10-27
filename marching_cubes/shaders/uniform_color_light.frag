#version 430
out vec4 FragColor;

in vec3 light;
uniform vec4 color;

in float yPos;
void main()
{
    //FragColor = vec4(color.xyz * light, color[3]);
    vec3 color = vec3(0);
    if (yPos < 3.4) {
        color.z = (yPos - 2.8) * 1;
    } else if (yPos < 3.8) {
        color.y = (yPos - 3.4) * 1;
    } else {
        color =  vec3((yPos - 4)*1, (yPos - 3.4)*1, 0);
    }
    FragColor = vec4(color * light, 1);
}