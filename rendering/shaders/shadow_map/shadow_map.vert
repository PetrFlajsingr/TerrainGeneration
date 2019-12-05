#version 430

out vec4 ShadowCoord;

void main()
{
    ShadowCoord= gl_TextureMatrix[7] * gl_Vertex;

    gl_Position = ftransform();

    gl_FrontColor = gl_Color;
}
