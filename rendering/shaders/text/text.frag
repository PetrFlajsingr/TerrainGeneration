#version 430

in vec2 uv;
uniform sampler2D texture;
uniform vec4 inputColor;

out vec4 color;

void main()
{
    color = vec4(inputColor.rgb, texture2D(texture, uv).a);
}