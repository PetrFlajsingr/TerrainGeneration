//
// Created by petr on 10/30/19.
//

#ifndef TERRAINGENERATION_FASTCHUNKGEN_H
#define TERRAINGENERATION_FASTCHUNKGEN_H

#include <geGL/geGL.h>

class FastChunkGen {
public:

  void loadShaders();

private:
  using Shader = std::shared_ptr<ge::gl::Shader>;
  using Program = GLuint;
  using Buffer = std::shared_ptr<ge::gl::Buffer>;
  using VertexArray = std::shared_ptr<ge::gl::VertexArray>;

  Shader generateDensity;
  Shader streamCases;
  Shader streamEdgeMarkers;
  Shader generateVertices;
  Shader clearVertexIDs;
  Shader splatVertices;
  Shader genIndices;

  Program generateDensityProgram;
  Program streamCasesProgram;
  Program streamEdgeMarkersProgram;
  Program generateVerticesProgram;
  Program clearVertexIDsProgram;
  Program splatVerticesProgram;
  Program generateIndicesProrgam;

  Buffer polyCountLUTBuffer;
  Buffer edgeLUTBuffer;
  Buffer edgeToVertexLUTBuffer;

  Buffer densityBuffer;
  Buffer vertexBuffer;
  Buffer vertexIDsBuffer;
  Buffer indexBuffer;

};

#endif // TERRAINGENERATION_FASTCHUNKGEN_H
