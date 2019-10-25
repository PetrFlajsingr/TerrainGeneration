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

uint computeVertexIndex(uint number, uint offset) {
    uint w = 8;
    uint h = 8;
    uint d = 8;
    switch (number) {
        case 0: return offset + w;
        case 1: return offset;
        case 2: return offset + 1;
        case 3: return offset + w + 1;
        case 4: return offset + w * h + w;
        case 5: return offset + w * h;
        case 6: return offset + w * h + 1;
        case 7: return offset + w * h + w + 1;
    }
    return 0;
}

void main() {
    if (gl_PrimitiveIDIn % 8 == 7 || gl_PrimitiveIDIn % 64 > 55 || gl_PrimitiveIDIn >= 448) return;

    gl_Position = mvpUniform * vertices[computeVertexIndex(0, gl_PrimitiveIDIn)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(1, gl_PrimitiveIDIn)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(1, gl_PrimitiveIDIn)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(2, gl_PrimitiveIDIn)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(2, gl_PrimitiveIDIn)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(3, gl_PrimitiveIDIn)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(4, gl_PrimitiveIDIn)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(5, gl_PrimitiveIDIn)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(5, gl_PrimitiveIDIn)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(6, gl_PrimitiveIDIn)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(6, gl_PrimitiveIDIn)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(7, gl_PrimitiveIDIn)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(4, gl_PrimitiveIDIn)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(7, gl_PrimitiveIDIn)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(0, gl_PrimitiveIDIn)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(4, gl_PrimitiveIDIn)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(1, gl_PrimitiveIDIn)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(5, gl_PrimitiveIDIn)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(2, gl_PrimitiveIDIn)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(6, gl_PrimitiveIDIn)];
    EmitVertex();
    EndPrimitive();

    gl_Position = mvpUniform * vertices[computeVertexIndex(3, gl_PrimitiveIDIn)];
    EmitVertex();
    gl_Position = mvpUniform * vertices[computeVertexIndex(7, gl_PrimitiveIDIn)];
    EmitVertex();
    EndPrimitive();
}
