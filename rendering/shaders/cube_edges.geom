#version 430

layout(points) in;
layout(line_strip, max_vertices = 24) out;

layout(binding=0, std430)buffer Vertices{ vec4 vertices[]; };
layout(location=1)uniform mat4 mvpUniform;

in gl_PerVertex
{
    vec4 gl_Position;
} gl_in[];

out gl_PerVertex
{
    vec4 gl_Position;
};

uint computeVertexIndex(uint number, uint offset, uint step) {
    uint w = 32;
    uint h = w;
    switch (number) {
        case 0: return offset + w * step;
        case 1: return offset;
        case 2: return offset + 1 * step;
        case 3: return offset + (w + 1) * step;
        case 4: return offset + (w * h + w) * step;
        case 5: return offset + (w * h) * step;
        case 6: return offset + (w * h + 1) * step;
        case 7: return offset + (w * h + w + 1) * step;
    }
    return 0;
}
uniform uint step;
void main() {
    uint dim = 32;
    if ((gl_PrimitiveIDIn % dim) == dim - 1 ||
(gl_PrimitiveIDIn % (dim * dim)) >= dim * (dim - 1) ||
(gl_PrimitiveIDIn % (dim * dim * dim)) >= dim * dim * (dim - 1)) return;

    if (
    gl_PrimitiveIDIn % step != 0
    || gl_PrimitiveIDIn / dim % step != 0
    || gl_PrimitiveIDIn / (dim * dim) % step != 0
    ) return;

    gl_Position = mvpUniform * vertices[computeVertexIndex(0, gl_PrimitiveIDIn, step)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(1, gl_PrimitiveIDIn, step)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(1, gl_PrimitiveIDIn, step)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(2, gl_PrimitiveIDIn, step)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(2, gl_PrimitiveIDIn, step)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(3, gl_PrimitiveIDIn, step)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(4, gl_PrimitiveIDIn, step)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(5, gl_PrimitiveIDIn, step)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(5, gl_PrimitiveIDIn, step)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(6, gl_PrimitiveIDIn, step)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(6, gl_PrimitiveIDIn, step)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(7, gl_PrimitiveIDIn, step)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(4, gl_PrimitiveIDIn, step)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(7, gl_PrimitiveIDIn, step)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(0, gl_PrimitiveIDIn, step)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(4, gl_PrimitiveIDIn, step)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(1, gl_PrimitiveIDIn, step)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(5, gl_PrimitiveIDIn, step)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(2, gl_PrimitiveIDIn, step)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(6, gl_PrimitiveIDIn, step)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(3, gl_PrimitiveIDIn, step)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(7, gl_PrimitiveIDIn, step)];
    EmitVertex();
    EndPrimitive();
}
