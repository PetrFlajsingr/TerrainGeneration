#version 430 
#extension GL_EXT_texture_array : enable
in vec2 v2fTexcoords;

layout(binding = 0) uniform sampler2DArray arrayTex;

uniform int layer;

layout(location = 0) out vec4 color;

void main()
{
    vec4 tex_coord = vec4(v2fTexcoords.x, v2fTexcoords.y, layer, 1.0);
    color = vec4(vec3(texture2DArray(arrayTex, tex_coord.xyz).x), 1.0f);
}