#version 430

in vec4 Position;
in vec3 Normal;

uniform mat4 projection, modelView;

out gl_PerVertex
{
    vec4 gl_Position;
};

out vec3 NormalInterp;
out vec3 Pos;
out vec3 posi;


void main () {
    posi = Position.xyz;
    gl_Position = projection * modelView * Position;
    Pos = (modelView * Position).xyz;
    mat4 normalMat = transpose(inverse(modelView));
    NormalInterp = vec3(normalMat * vec4(Normal, 1.0));
}